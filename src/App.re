[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

let defaultResult = Utils.OutputCode "";

type compilerState = {
  code: string,
  result: Utils.compilerResult,
  compiling: bool
};

type state = {
  reason: compilerState,
  jsxv2: compilerState,
  ocaml: compilerState
};

type action =
  | CompileReason string
  | UpdateRefmtResult Utils.compilerResult
  | RewriteJsxV2 string
  | UpdateJsxV2Result Utils.compilerResult
  | CompileOCaml string
  | UpdateBucklescriptResult Utils.compilerResult
  | UpdateAll (Utils.compilerResult, Utils.compilerResult, Utils.compilerResult);

let component = ReasonReact.reducerComponent "App";

let test_reason_code = {j|module Greeting = {
  let component = ReasonReact.statelessComponent "Greeting";

  let make _children => {
    ...component,
    render: fun self => <button> (ReasonReact.stringToElement "Hello!") </button>
  };
};
ReactDOMRe.renderToElementWithId <Greeting /> "preview";|j};

let make _children => {
  ...component,
  initialState: fun () => {
    let defaultCompilerState = {code: "", result: defaultResult, compiling: false};
    {
      reason: {...defaultCompilerState, code: test_reason_code},
      jsxv2: defaultCompilerState,
      ocaml: defaultCompilerState
    }
  },
  reducer: fun action state => {
    let getCode result =>
      switch result {
      | Utils.OutputCode code => code
      | Utils.ErrorMessage _ => ""
      };
    switch action {
    | CompileReason code =>
      ReasonReact.Update {...state, reason: {...state.reason, code, compiling: true}}
    | UpdateRefmtResult result =>
      ReasonReact.Update {
        ...state,
        reason: {...state.reason, result, compiling: false},
        jsxv2: {...state.jsxv2, code: getCode result}
      }
    | RewriteJsxV2 code =>
      ReasonReact.Update {...state, jsxv2: {...state.jsxv2, code, compiling: true}}
    | UpdateJsxV2Result result =>
      ReasonReact.Update {
        ...state,
        jsxv2: {...state.jsxv2, result, compiling: false},
        ocaml: {...state.ocaml, code: getCode result}
      }
    | CompileOCaml code =>
      ReasonReact.Update {...state, ocaml: {...state.ocaml, code, compiling: true}}
    | UpdateBucklescriptResult result =>
      ReasonReact.Update {...state, ocaml: {...state.ocaml, result, compiling: false}}
    | UpdateAll (reasonResult, jsxv2Result, ocamlResult) =>
      ReasonReact.Update {
        reason: {...state.reason, result: reasonResult, compiling: false},
        jsxv2: {code: getCode reasonResult, result: jsxv2Result, compiling: false},
        ocaml: {code: getCode jsxv2Result, result: ocamlResult, compiling: false}
      }
    }
  },
  render: fun self => {
    let logo = <Logo />;
    let compileOCaml2Js code => {
      let ocamlResult = Utils.compileOCaml code;
      (self.state.reason.result, self.state.jsxv2.result, ocamlResult)
    };
    let rewriteOCaml2Js code => {
      let rewriteResult = Utils.jsxv2Rewrite code;
      let ocamlResult =
        switch rewriteResult {
        | Utils.OutputCode ocamlCode => Utils.compileOCaml ocamlCode
        | Utils.ErrorMessage _ => defaultResult
        };
      (self.state.reason.result, rewriteResult, ocamlResult)
    };
    let compileReason2Js code => {
      let reasonResult = Utils.compileReason code;
      let (rewriteResult, ocamlResult) =
        switch reasonResult {
        | Utils.OutputCode jsxv2Code =>
          let (_, rewriteResult, ocamlResult) = rewriteOCaml2Js jsxv2Code;
          (rewriteResult, ocamlResult)
        | Utils.ErrorMessage _ => (defaultResult, defaultResult)
        };
      (reasonResult, rewriteResult, ocamlResult)
    };
    let onReasonChange code _change =>
      self.reduce (fun _event => UpdateAll (compileReason2Js code)) ();
    let debouncedOnReasonChange = Utils.debounce onReasonChange wait::250.0;
    let onOCamlJsxChange code _change =>
      self.reduce (fun _event => UpdateAll (rewriteOCaml2Js code)) ();
    let debouncedOnOCamlJsxChange = Utils.debounce onOCamlJsxChange wait::250.0;
    let onOCamlChange code _change =>
      self.reduce (fun _event => UpdateAll (compileOCaml2Js code)) ();
    let debouncedOnOCamlChange = Utils.debounce onOCamlChange wait::250.0;
    let getError compilerResult =>
      switch compilerResult {
      | Utils.OutputCode _ => None
      | Utils.ErrorMessage e => Some e
      };
    let (jsCode, jsErrorMessage) =
      switch self.state.ocaml.result {
      | Utils.OutputCode jsCode => (jsCode, "")
      | Utils.ErrorMessage errorMessage => ("", errorMessage)
      };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar title="Reason React Playground" leftIcon=logo />
        <section>
          <CodeEditor
            label="Reason"
            mode="rust"
            code=self.state.reason.code
            error=?(getError self.state.reason.result)
            onChange=debouncedOnReasonChange
          />
          <CodeEditor
            label="OCaml+JSX"
            mode="mllike"
            code=self.state.jsxv2.code
            error=?(getError self.state.jsxv2.result)
            onChange=debouncedOnOCamlJsxChange
          />
          <CodeEditor
            label="OCaml"
            mode="mllike"
            code=self.state.ocaml.code
            error=?(getError self.state.ocaml.result)
            onChange=debouncedOnOCamlChange
          />
          <ReactToolbox.Input
            _type="text"
            multiline=true
            label=(ReasonReact.stringToElement "JS")
            value=(jsCode ^ jsErrorMessage)
          />
          <Preview code=jsCode />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
