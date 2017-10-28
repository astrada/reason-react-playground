[%bs.raw {|require('./toolbox/theme.css')|}];

[@bs.module] external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme";

let defaultStorageKey = "default-reason-code";

let defaultResult = Utils.OutputCode("");

type compilerState = {
  code: string,
  result: Utils.compilerResult,
  compiling: bool,
  show: bool
};

type state = {
  loading: bool,
  reason: compilerState,
  jsxv2: compilerState,
  ocaml: compilerState,
  showJs: bool
};

type action =
  | UpdateAll((string, Utils.compilerResult, Utils.compilerResult, Utils.compilerResult))
  | ToggleJsxV2
  | ToggleOCaml
  | ToggleJs
  | ShowJs;

let component = ReasonReact.reducerComponent("App");

let defaultReasonCode = {j|module Greeting = {
  let component = ReasonReact.statelessComponent("Greeting");
  let make = (_children) => {
    ...component,
    render: (self) => <button> (ReasonReact.stringToElement("Hello!")) </button>
  };
};

ReactDOMRe.renderToElementWithId(<Greeting />, "preview");|j};

let getCode = (result) =>
  switch result {
  | Utils.OutputCode(code) => code
  | Utils.ErrorMessage(_) => ""
  };

let getError = (compilerResult) =>
  switch compilerResult {
  | Utils.OutputCode(_) => None
  | Utils.ErrorMessage(e) => Some(e)
  };

let hasError = (compilerResult) =>
  switch compilerResult {
  | Utils.OutputCode(_) => false
  | Utils.ErrorMessage(_) => true
  };

let initialVow = {
  let reasonCodeVow = LocalForage.getItem(defaultStorageKey);
  Vow.Result.map(
    (o) => {
      let reasonCode =
        switch o {
        | None => defaultReasonCode
        | Some(code) => code
        };
      let refmtResult = Utils.refmtRE2ML(reasonCode);
      let jsxCode = getCode(refmtResult);
      let jsxv2Result = Utils.jsxv2Rewrite(jsxCode);
      let ocamlCode = getCode(jsxv2Result);
      let ocamlResult = Utils.compileOCaml(ocamlCode);
      Vow.Result.return((reasonCode, refmtResult, jsxv2Result, ocamlResult))
    },
    reasonCodeVow
  )
};

let make = (_children) => {
  ...component,
  initialState: () => {
    let defaultCompilerState = {code: "", result: defaultResult, compiling: false, show: false};
    {
      loading: true,
      reason: defaultCompilerState,
      jsxv2: defaultCompilerState,
      ocaml: defaultCompilerState,
      showJs: false
    }
  },
  didMount: (self) => {
    if (self.state.loading) {
      Vow.Result.sideEffect(
        fun
        | `Fail () => ()
        | `Success(reasonCode, refmtResult, jsxv2Result, ocamlResult) =>
          self.reduce(() => UpdateAll((reasonCode, refmtResult, jsxv2Result, ocamlResult)), ()),
        initialVow
      )
    };
    ReasonReact.NoUpdate
  },
  reducer: (action, state) =>
    switch action {
    | UpdateAll((reasonCode, reasonResult, jsxv2Result, ocamlResult)) =>
      let jsxv2Code = getCode(reasonResult);
      let jsxv2Code =
        if (jsxv2Code == "") {
          state.jsxv2.code
        } else {
          jsxv2Code
        };
      let ocamlCode = getCode(jsxv2Result);
      let ocamlCode =
        if (ocamlCode == "") {
          state.ocaml.code
        } else {
          ocamlCode
        };
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          loading: false,
          reason: {...state.reason, code: reasonCode, result: reasonResult, compiling: false},
          jsxv2: {
            code: jsxv2Code,
            result: jsxv2Result,
            compiling: false,
            show: state.jsxv2.show || hasError(jsxv2Result)
          },
          ocaml: {
            code: ocamlCode,
            result: ocamlResult,
            compiling: false,
            show: state.ocaml.show || hasError(ocamlResult)
          }
        },
        (
          (_self) => {
            let vow = LocalForage.setItem(defaultStorageKey, reasonCode);
            Vow.Result.sideEffect((_) => (), vow)
          }
        )
      )
    | ToggleJsxV2 =>
      ReasonReact.Update({...state, jsxv2: {...state.jsxv2, show: ! state.jsxv2.show}})
    | ToggleOCaml =>
      ReasonReact.Update({...state, ocaml: {...state.ocaml, show: ! state.ocaml.show}})
    | ToggleJs => ReasonReact.Update({...state, showJs: ! state.showJs})
    | ShowJs => ReasonReact.Update({...state, showJs: true})
    },
  render: (self) => {
    let logo = <Logo />;
    let githubIcon = <GithubIcon />;
    let rewriteJsxV2 = (code) => {
      let rewriteResult = Utils.jsxv2Rewrite(code);
      let ocamlResult =
        switch rewriteResult {
        | Utils.OutputCode(ocamlCode) => Utils.compileOCaml(ocamlCode)
        | Utils.ErrorMessage(_) => defaultResult
        };
      (self.state.reason.result, rewriteResult, ocamlResult)
    };
    let refmtRE2ML = (code) => {
      let reasonResult = Utils.refmtRE2ML(code);
      let (rewriteResult, ocamlResult) =
        switch reasonResult {
        | Utils.OutputCode(jsxv2Code) =>
          let (_, rewriteResult, ocamlResult) = rewriteJsxV2(jsxv2Code);
          (rewriteResult, ocamlResult)
        | Utils.ErrorMessage(_) => (defaultResult, defaultResult)
        };
      (code, reasonResult, rewriteResult, ocamlResult)
    };
    let refmtML2RE = (code) => {
      let (_, rewriteResult, ocamlResult) = rewriteJsxV2(code);
      let reasonResult = Utils.refmtML2RE(code);
      let reasonCode =
        switch reasonResult {
        | Utils.OutputCode(code) => code
        | Utils.ErrorMessage(_) => self.state.reason.code
        };
      (reasonCode, self.state.reason.result, rewriteResult, ocamlResult)
    };
    let onReasonChange = (code, _change) =>
      self.reduce((_event) => UpdateAll(refmtRE2ML(code)), ());
    let debouncedOnReasonChange = Utils.debounce(onReasonChange, ~wait=250.0);
    let onOCamlJsxChange = (code, _change) =>
      self.reduce((_event) => UpdateAll(refmtML2RE(code)), ());
    let debouncedOnOCamlJsxChange = Utils.debounce(onOCamlJsxChange, ~wait=250.0);
    let (jsCode, jsErrorMessage) =
      switch self.state.ocaml.result {
      | Utils.OutputCode(jsCode) => (jsCode, None)
      | Utils.ErrorMessage(errorMessage) => ("", Some(errorMessage))
      };
    let buildCodeEditor = (~onChange=?, compilerState, label, mode) =>
      if (compilerState.show) {
        let readOnly = Js.Option.isNone(onChange);
        <CodeEditor
          label
          mode
          code=compilerState.code
          error=?(getError(compilerState.result))
          ?onChange
          readOnly
        />
      } else {
        ReasonReact.nullElement
      };
    let jsxv2CodeEditor =
      buildCodeEditor(
        ~onChange=debouncedOnOCamlJsxChange,
        self.state.jsxv2,
        "OCaml+JSX",
        "mllike"
      );
    let ocamlCodeEditor = buildCodeEditor(self.state.ocaml, "OCaml", "mllike");
    let jsCodeEditor =
      if (self.state.showJs) {
        <CodeEditor label="JS" mode="javascript" code=jsCode error=?jsErrorMessage readOnly=true />
      } else {
        ReasonReact.nullElement
      };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar
          title="Reason React Playground"
          leftIcon=logo
          onLeftIconClick=(
            (_event) => Utils.redirect("https://astrada.github.io/reason-react-playground/")
          )
          rightIcon=githubIcon
          onRightIconClick=(
            (_event) => Utils.openWindow("https://github.com/astrada/reason-react-playground/")
          )
        />
        <section>
          <CodeEditor
            label="Reason"
            mode="rust"
            loading=self.state.loading
            code=self.state.reason.code
            error=?(getError(self.state.reason.result))
            onChange=debouncedOnReasonChange
          />
          <ReactToolbox.Checkbox
            checked=self.state.jsxv2.show
            label=(ReasonReact.stringToElement("Show OCaml+JSX"))
            onChange=((_checked, event) => self.reduce((_event) => ToggleJsxV2, event))
          />
          jsxv2CodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.ocaml.show
            label=(ReasonReact.stringToElement("Show OCaml (read-only)"))
            onChange=((_checked, event) => self.reduce((_event) => ToggleOCaml, event))
          />
          ocamlCodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.showJs
            label=(ReasonReact.stringToElement("Show JS (read-only)"))
            onChange=((_checked, event) => self.reduce((_event) => ToggleJs, event))
          />
          jsCodeEditor
          <Preview
            code=jsCode
            onDone=(
              (error) =>
                if (error) {
                  self.reduce((_) => ShowJs, ())
                }
            )
          />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
