[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

type state = {
  reasonCode: string,
  refmtResult: Utils.compilerResult,
  ocamlCode: string,
  bucklescriptResult: Utils.compilerResult,
  compiling: bool
};

type action =
  | CompileReason string
  | UpdateRefmtResult Utils.compilerResult
  | CompileOCaml string
  | UpdateBucklescriptResult Utils.compilerResult;

let component = ReasonReact.reducerComponent "App";

let make _children => {
  ...component,
  initialState: fun () => {
    reasonCode: "",
    refmtResult: Utils.OutputCode "",
    ocamlCode: "",
    bucklescriptResult: Utils.OutputCode "",
    compiling: false
  },
  reducer: fun action state =>
    switch action {
    | CompileReason reasonCode => ReasonReact.Update {...state, reasonCode, compiling: true}
    | UpdateRefmtResult refmtResult =>
      let ocamlCode =
        switch refmtResult {
        | Utils.OutputCode code => code
        | _ => ""
        };
      ReasonReact.Update {...state, ocamlCode, refmtResult, compiling: false}
    | CompileOCaml ocamlCode => ReasonReact.Update {...state, ocamlCode, compiling: true}
    | UpdateBucklescriptResult bucklescriptResult =>
      ReasonReact.Update {...state, compiling: false, bucklescriptResult}
    },
  render: fun self => {
    let logo = <Logo />;
    let compileReason code event => {
      /*Utils.compileReason
          code
          (
            fun compilerResult => {
              let reduce = self.reduce (fun _event => UpdateRefmtResult compilerResult);
              reduce event
            }
          );
        let reduce = self.reduce (fun _event => CompileReason code);
        reduce event*/
      let reduce = self.reduce (fun _event => UpdateRefmtResult (Utils.OutputCode code));
      reduce event
    };
    let compileOCaml code event => {
      /*Utils.compileOCaml
          code
          (
            fun compilerResult => {
              let reduce = self.reduce (fun _event => UpdateBucklescriptResult compilerResult);
              reduce event
            }
          );
        let reduce = self.reduce (fun _event => CompileReason code);
        reduce event*/
      let reduce = self.reduce (fun _event => UpdateBucklescriptResult (Utils.OutputCode code));
      reduce event
    };
    let (jsCode, jsErrorMessage) =
      switch self.state.bucklescriptResult {
      | Utils.OutputCode jsCode => (jsCode, "")
      | Utils.ErrorMessage errorMessage => ("", errorMessage)
      };
    let onReasonChange code _change => compileReason code ();
    let debouncedOnReasonChange = Utils.debounce onReasonChange wait::500.0;
    let onOCamlChange code _change => compileOCaml code ();
    let debouncedOnOCamlChange = Utils.debounce onOCamlChange wait::500.0;
    let onch1 code _ => {
      let reduce = self.reduce (fun _event => CompileReason code);
      reduce ()
    };
    let onch2 code _ => {
      let reduce = self.reduce (fun _event => CompileOCaml code);
      reduce ()
    };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar title="App example" leftIcon=logo />
        <section>
          <CodeEditor
            label="Reason"
            mode="rust"
            code=self.state.reasonCode
            onChange=onch1
            /*onChange=debouncedOnReasonChange*/
          />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=(compileReason self.state.reasonCode)
            /*disabled=self.state.compiling*/
          />
          <CodeEditor
            label="OCaml"
            mode="mllike"
            code=self.state.ocamlCode
            onChange=onch2
            /*onChange=debouncedOnOCamlChange*/
          />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=(compileOCaml self.state.ocamlCode)
            /*disabled=self.state.compiling*/
          />
          <ReactToolbox.Input
            _type="text"
            multiline=true
            label=(ReasonReact.stringToElement "JS")
            value=(jsCode ^ jsErrorMessage)
          />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
