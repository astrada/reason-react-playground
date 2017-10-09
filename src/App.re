[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

let defaultResult = Utils.OutputCode "";

type compilerState = {
  code: string,
  result: Utils.compilerResult,
  compiling: bool,
  show: bool
};

type state = {
  reason: compilerState,
  jsxv2: compilerState,
  ocaml: compilerState,
  showJs: bool
};

type action =
  | CompileReason string
  | UpdateRefmtResult Utils.compilerResult
  | RewriteJsxV2 string
  | UpdateJsxV2Result Utils.compilerResult
  | CompileOCaml string
  | UpdateBucklescriptResult Utils.compilerResult
  | UpdateAll (string, Utils.compilerResult, Utils.compilerResult, Utils.compilerResult)
  | ToggleJsxV2
  | ToggleOCaml
  | ToggleJs
  | ShowJs;

let component = ReasonReact.reducerComponent "App";

let initialReasonCode = {j|module Greeting = {
  let component = ReasonReact.statelessComponent "Greeting";

  let make _children => {
    ...component,
    render: fun self => <button> (ReasonReact.stringToElement "Hello!") </button>
  };
};

ReactDOMRe.renderToElementWithId <Greeting /> "preview";|j};

let getCode result =>
  switch result {
  | Utils.OutputCode code => code
  | Utils.ErrorMessage _ => ""
  };

let getError compilerResult =>
  switch compilerResult {
  | Utils.OutputCode _ => None
  | Utils.ErrorMessage e => Some e
  };

let hasError compilerResult =>
  switch compilerResult {
  | Utils.OutputCode _ => false
  | Utils.ErrorMessage _ => true
  };

let initialReasonResult = Utils.refmtRE2ML initialReasonCode;

let initialJsxV2Code = getCode initialReasonResult;

let initialJsxV2Result = Utils.jsxv2Rewrite initialJsxV2Code;

let initialOCamlCode = getCode initialJsxV2Result;

let initialOCamlResult = Utils.compileOCaml initialOCamlCode;

let make _children => {
  ...component,
  initialState: fun () => {
    let defaultCompilerState = {code: "", result: defaultResult, compiling: false, show: false};
    {
      reason: {...defaultCompilerState, code: initialReasonCode, show: true},
      jsxv2: {...defaultCompilerState, code: initialJsxV2Code, result: initialJsxV2Result},
      ocaml: {...defaultCompilerState, code: initialOCamlCode, result: initialOCamlResult},
      showJs: false
    }
  },
  reducer: fun action state =>
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
    | UpdateAll (reasonCode, reasonResult, jsxv2Result, ocamlResult) =>
      let jsxv2Code = getCode reasonResult;
      let jsxv2Code =
        if (jsxv2Code == "") {
          state.jsxv2.code
        } else {
          jsxv2Code
        };
      let ocamlCode = getCode jsxv2Result;
      let ocamlCode =
        if (ocamlCode == "") {
          state.ocaml.code
        } else {
          ocamlCode
        };
      ReasonReact.Update {
        ...state,
        reason: {...state.reason, code: reasonCode, result: reasonResult, compiling: false},
        jsxv2: {
          code: jsxv2Code,
          result: jsxv2Result,
          compiling: false,
          show: state.jsxv2.show || hasError jsxv2Result
        },
        ocaml: {
          code: ocamlCode,
          result: ocamlResult,
          compiling: false,
          show: state.ocaml.show || hasError ocamlResult
        }
      }
    | ToggleJsxV2 =>
      ReasonReact.Update {...state, jsxv2: {...state.jsxv2, show: not state.jsxv2.show}}
    | ToggleOCaml =>
      ReasonReact.Update {...state, ocaml: {...state.ocaml, show: not state.ocaml.show}}
    | ToggleJs => ReasonReact.Update {...state, showJs: not state.showJs}
    | ShowJs => ReasonReact.Update {...state, showJs: true}
    },
  render: fun self => {
    let logo = <Logo />;
    let githubIcon = <GithubIcon />;
    let rewriteJsxV2 code => {
      let rewriteResult = Utils.jsxv2Rewrite code;
      let ocamlResult =
        switch rewriteResult {
        | Utils.OutputCode ocamlCode => Utils.compileOCaml ocamlCode
        | Utils.ErrorMessage _ => defaultResult
        };
      (self.state.reason.result, rewriteResult, ocamlResult)
    };
    let refmtRE2ML code => {
      let reasonResult = Utils.refmtRE2ML code;
      let (rewriteResult, ocamlResult) =
        switch reasonResult {
        | Utils.OutputCode jsxv2Code =>
          let (_, rewriteResult, ocamlResult) = rewriteJsxV2 jsxv2Code;
          (rewriteResult, ocamlResult)
        | Utils.ErrorMessage _ => (defaultResult, defaultResult)
        };
      (self.state.reason.code, reasonResult, rewriteResult, ocamlResult)
    };
    let refmtML2RE code => {
      let (_, rewriteResult, ocamlResult) = rewriteJsxV2 code;
      let reasonResult = Utils.refmtML2RE code;
      let reasonCode =
        switch reasonResult {
        | Utils.OutputCode code => code
        | Utils.ErrorMessage _ => self.state.reason.code
        };
      (reasonCode, self.state.reason.result, rewriteResult, ocamlResult)
    };
    let onReasonChange code _change => self.reduce (fun _event => UpdateAll (refmtRE2ML code)) ();
    let debouncedOnReasonChange = Utils.debounce onReasonChange wait::250.0;
    let onOCamlJsxChange code _change =>
      self.reduce (fun _event => UpdateAll (refmtML2RE code)) ();
    let debouncedOnOCamlJsxChange = Utils.debounce onOCamlJsxChange wait::250.0;
    let (jsCode, jsErrorMessage) =
      switch self.state.ocaml.result {
      | Utils.OutputCode jsCode => (jsCode, None)
      | Utils.ErrorMessage errorMessage => ("", Some errorMessage)
      };
    let buildCodeEditor ::onChange=? compilerState label mode =>
      if compilerState.show {
        let readOnly = Js.Option.isNone onChange;
        <CodeEditor
          label
          mode
          code=compilerState.code
          error=?(getError compilerState.result)
          onChange=?onChange
          readOnly
        />
      } else {
        ReasonReact.nullElement
      };
    let jsxv2CodeEditor =
      buildCodeEditor onChange::debouncedOnOCamlJsxChange self.state.jsxv2 "OCaml+JSX" "mllike";
    let ocamlCodeEditor = buildCodeEditor self.state.ocaml "OCaml" "mllike";
    let jsCodeEditor =
      if self.state.showJs {
        <CodeEditor label="JS" mode="javascript" code=jsCode error=?jsErrorMessage readOnly=true />
      } else {
        ReasonReact.nullElement
      };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar
          title="Reason React Playground"
          leftIcon=logo
          onLeftIconClick=(fun _event => Utils.redirect "/")
          rightIcon=githubIcon
          onRightIconClick=(
            fun _event => Utils.openWindow "https://github.com/astrada/reason-react-playground/"
          )
        />
        <section>
          <CodeEditor
            label="Reason"
            mode="rust"
            code=self.state.reason.code
            error=?(getError self.state.reason.result)
            onChange=debouncedOnReasonChange
          />
          <ReactToolbox.Checkbox
            checked=self.state.jsxv2.show
            label=(ReasonReact.stringToElement "Show OCaml+JSX")
            onChange=(fun _checked event => self.reduce (fun _event => ToggleJsxV2) event)
          />
          jsxv2CodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.ocaml.show
            label=(ReasonReact.stringToElement "Show OCaml (read-only)")
            onChange=(fun _checked event => self.reduce (fun _event => ToggleOCaml) event)
          />
          ocamlCodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.showJs
            label=(ReasonReact.stringToElement "Show JS (read-only)")
            onChange=(fun _checked event => self.reduce (fun _event => ToggleJs) event)
          />
          jsCodeEditor
          <Preview
            code=jsCode
            onDone=(
              fun error =>
                if error {
                  self.reduce (fun _ => ShowJs) ()
                }
            )
          />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
