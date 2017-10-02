[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

type state = {
  reasonCode: string,
  refmtResult: Utils.compilerResult,
  compilingReason: bool,
  ocamlCode: string,
  bucklescriptResult: Utils.compilerResult,
  compilingOCaml: bool
};

type action =
  | CompileReason string
  | UpdateRefmtResult Utils.compilerResult
  | CompileOCaml string
  | UpdateBucklescriptResult Utils.compilerResult;

let component = ReasonReact.reducerComponent "App";

let test_reason_code = {j|module Greeting = {
  let component = ReasonReact.statelessComponent "Greeting";

  /* underscore before names indicate unused variables. We name them for clarity */
  let make _children => {
    ...component,
    render: fun self => <button> (ReasonReact.stringToElement "Hello!") </button>
  };
};
ReactDOMRe.renderToElementWithId <Greeting /> "preview";|j};

let make _children => {
  ...component,
  initialState: fun () => {
    reasonCode: test_reason_code,
    refmtResult: Utils.OutputCode "",
    compilingReason: false,
    ocamlCode: "",
    bucklescriptResult: Utils.OutputCode "",
    compilingOCaml: false
  },
  reducer: fun action state =>
    switch action {
    | CompileReason reasonCode => ReasonReact.Update {...state, reasonCode, compilingReason: true}
    | UpdateRefmtResult refmtResult =>
      let ocamlCode =
        switch refmtResult {
        | Utils.OutputCode code => code
        | _ => ""
        };
      ReasonReact.Update {...state, refmtResult, compilingReason: false, ocamlCode}
    | CompileOCaml ocamlCode => ReasonReact.Update {...state, ocamlCode, compilingOCaml: true}
    | UpdateBucklescriptResult bucklescriptResult =>
      ReasonReact.Update {...state, bucklescriptResult, compilingOCaml: false}
    },
  render: fun self => {
    let logo = <Logo />;
    let compileOCaml code event => {
      Utils.jsxv3Rewrite
        code
        (
          fun compilerResult =>
            switch compilerResult {
            | Utils.OutputCode ocamlCode =>
              Utils.compileOCaml
                ocamlCode
                (
                  fun compilerResult => {
                    let reduce =
                      self.reduce (fun _event => UpdateBucklescriptResult compilerResult);
                    reduce event
                  }
                )
            | Utils.ErrorMessage _ =>
              let reduce = self.reduce (fun _event => UpdateBucklescriptResult compilerResult);
              reduce event
            }
        );
      let reduce = self.reduce (fun _event => CompileOCaml code);
      reduce event
    };
    let compileReason code event => {
      Utils.compileReason
        code
        (
          fun compilerResult => {
            let reduce = self.reduce (fun _event => UpdateRefmtResult compilerResult);
            reduce event;
            switch compilerResult {
            | Utils.OutputCode ocamlCode => compileOCaml ocamlCode ()
            | _ => ()
            }
          }
        );
      let reduce = self.reduce (fun _event => CompileReason code);
      reduce event
    };
    let onReasonChange code _change => compileReason code ();
    let debouncedOnReasonChange = Utils.debounce onReasonChange wait::1000.0;
    let onOCamlChange code _change => compileOCaml code ();
    let debouncedOnOCamlChange = Utils.debounce onOCamlChange wait::1000.0;
    let getError compilerResult =>
      switch compilerResult {
      | Utils.OutputCode _ => None
      | Utils.ErrorMessage e => Some e
      };
    let (jsCode, jsErrorMessage) =
      switch self.state.bucklescriptResult {
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
            code=self.state.reasonCode
            error=?(getError self.state.refmtResult)
            onChange=debouncedOnReasonChange
          />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=(compileReason self.state.reasonCode)
            disabled=self.state.compilingReason
          />
          <CodeEditor
            label="OCaml"
            mode="mllike"
            code=self.state.ocamlCode
            error=?(getError self.state.bucklescriptResult)
            onChange=debouncedOnOCamlChange
          />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=(compileOCaml self.state.ocamlCode)
            disabled=self.state.compilingOCaml
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
