[%bs.raw {|require('./toolbox/theme.css')|}];

[@bs.module]
external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme";

let defaultResult = Utils.OutputCode("", None);

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
  | UpdateReason(
      (
        string,
        Utils.compilerResult,
        Utils.compilerResult,
        Utils.compilerResult
      )
    )
  | UpdateOCamlJsx(
      (
        string,
        Utils.compilerResult,
        Utils.compilerResult,
        Utils.compilerResult
      )
    )
  | ToggleJsxV2
  | ToggleOCaml
  | ToggleJs
  | ShowJs
  | Reset;

let component = ReasonReact.reducerComponent("App");

let defaultReasonCode = {j|module Greeting = {
  let component = ReasonReact.statelessComponent("Greeting");
  let make = (_children) => {
    ...component,
    render: (self) => <button> (ReasonReact.stringToElement("Hello!")) </button>
  };
};

ReactDOMRe.renderToElementWithId(<Greeting />, "preview");|j};

let getCode = result =>
  switch (result) {
  | Utils.OutputCode(code, _) => code
  | Utils.ErrorMessage(_) => ""
  };

let getWarnings = result =>
  switch (result) {
  | Utils.OutputCode(_, warnings) => warnings
  | Utils.ErrorMessage(_) => None
  };

let hasWarnings = compilerResult =>
  switch (compilerResult) {
  | Utils.OutputCode(_, Some(_)) => true
  | Utils.OutputCode(_, None)
  | Utils.ErrorMessage(_) => false
  };

let getError = compilerResult =>
  switch (compilerResult) {
  | Utils.OutputCode(_) => None
  | Utils.ErrorMessage(e) => Some(e)
  };

let hasError = compilerResult =>
  switch (compilerResult) {
  | Utils.OutputCode(_) => false
  | Utils.ErrorMessage(_) => true
  };

let refmtRE2ML = reasonCode => {
  let refmtResult = Utils.refmtRE2ML(reasonCode);
  let ocamlJsxCode = getCode(refmtResult);
  let jsxv2Result =
    switch (refmtResult) {
    | Utils.OutputCode(_) => Utils.jsxv2Rewrite(ocamlJsxCode)
    | Utils.ErrorMessage(_) => defaultResult
    };
  let ocamlCode = getCode(jsxv2Result);
  let ocamlResult =
    switch (jsxv2Result) {
    | Utils.OutputCode(_) => Utils.compileOCaml(ocamlCode)
    | Utils.ErrorMessage(_) => defaultResult
    };
  (
    reasonCode,
    refmtResult,
    jsxv2Result,
    ocamlResult,
    Persister.Reason(reasonCode)
  );
};

let refmtML2RE = ocamlJsxCode => {
  let refmtResult = Utils.refmtML2RE(ocamlJsxCode);
  let reasonCode = getCode(refmtResult);
  let jsxv2Result = Utils.jsxv2Rewrite(ocamlJsxCode);
  let ocamlCode = getCode(jsxv2Result);
  let ocamlResult =
    switch (jsxv2Result) {
    | Utils.OutputCode(_) => Utils.compileOCaml(ocamlCode)
    | Utils.ErrorMessage(_) => defaultResult
    };
  (
    reasonCode,
    Utils.OutputCode(ocamlJsxCode, None),
    jsxv2Result,
    ocamlResult,
    Persister.OCamlJsx(ocamlJsxCode)
  );
};

let initialVow = {
  let initialCodeVow = Persister.loadPersistedState();
  Vow.Result.flatMap(
    fun
    | Persister.Null => Vow.Result.return(refmtRE2ML(defaultReasonCode))
    | Persister.Reason(reasonCode) => Vow.Result.return(refmtRE2ML(reasonCode))
    | Persister.OCamlJsx(ocamlJsxCode) =>
      Vow.Result.return(refmtML2RE(ocamlJsxCode)),
    initialCodeVow
  );
};

let make = _children => {
  ...component,
  initialState: () => {
    let defaultCompilerState = {
      code: "",
      result: defaultResult,
      compiling: false,
      show: false
    };
    {
      loading: true,
      reason: defaultCompilerState,
      jsxv2: defaultCompilerState,
      ocaml: defaultCompilerState,
      showJs: false
    };
  },
  didMount: self => {
    if (self.state.loading) {
      Vow.Result.sideEffect(
        fun
        | `Fail () => ()
        | `Success(
            reasonCode,
            refmtResult,
            jsxv2Result,
            ocamlResult,
            persisterCode
          ) =>
          switch (persisterCode) {
          | Persister.Null => ()
          | Persister.Reason(_) =>
            self.send(
              UpdateReason((reasonCode, refmtResult, jsxv2Result, ocamlResult))
            )
          | Persister.OCamlJsx(_) =>
            self.send(
              UpdateOCamlJsx((
                reasonCode,
                refmtResult,
                jsxv2Result,
                ocamlResult
              ))
            )
          },
        initialVow
      );
    };
    ReasonReact.NoUpdate;
  },
  reducer: (action, state) => {
    let updateAll =
        (reasonCode, reasonResult, jsxv2Result, ocamlResult, codeToSave) => {
      let jsxv2Code = getCode(reasonResult);
      let jsxv2Code =
        if (jsxv2Code == "") {
          state.jsxv2.code;
        } else {
          jsxv2Code;
        };
      let ocamlCode = getCode(jsxv2Result);
      let ocamlCode =
        if (ocamlCode == "") {
          state.ocaml.code;
        } else {
          ocamlCode;
        };
      let editingOCamlJsx =
        switch (codeToSave) {
        | Persister.OCamlJsx(_) => true
        | _ => false
        };
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          loading: false,
          reason: {
            ...state.reason,
            code: reasonCode,
            result: reasonResult,
            compiling: false
          },
          jsxv2: {
            code: jsxv2Code,
            result: jsxv2Result,
            compiling: false,
            show: state.jsxv2.show || editingOCamlJsx || hasError(jsxv2Result)
          },
          ocaml: {
            code: ocamlCode,
            result: ocamlResult,
            compiling: false,
            show:
              state.ocaml.show
              || hasError(ocamlResult)
              || hasWarnings(ocamlResult)
          }
        },
        _self => Persister.saveState(codeToSave)
      );
    };
    switch (action) {
    | UpdateReason((reasonCode, reasonResult, jsxv2Result, ocamlResult)) =>
      updateAll(
        reasonCode,
        reasonResult,
        jsxv2Result,
        ocamlResult,
        Persister.Reason(reasonCode)
      )
    | UpdateOCamlJsx((reasonCode, reasonResult, jsxv2Result, ocamlResult)) =>
      updateAll(
        reasonCode,
        reasonResult,
        jsxv2Result,
        ocamlResult,
        Persister.OCamlJsx(getCode(reasonResult))
      )
    | ToggleJsxV2 =>
      ReasonReact.Update({
        ...state,
        jsxv2: {
          ...state.jsxv2,
          show: ! state.jsxv2.show
        }
      })
    | ToggleOCaml =>
      ReasonReact.Update({
        ...state,
        ocaml: {
          ...state.ocaml,
          show: ! state.ocaml.show
        }
      })
    | ToggleJs => ReasonReact.Update({...state, showJs: ! state.showJs})
    | ShowJs => ReasonReact.Update({...state, showJs: true})
    | Reset =>
      let (reasonCode, reasonResult, jsxv2Result, ocamlResult, codeToSave) =
        refmtRE2ML(defaultReasonCode);
      updateAll(
        reasonCode,
        reasonResult,
        jsxv2Result,
        ocamlResult,
        codeToSave
      );
    };
  },
  render: self => {
    let logo = <Logo />;
    let githubIcon = <GithubIcon />;
    let rewriteJsxV2 = code => {
      let rewriteResult = Utils.jsxv2Rewrite(code);
      let ocamlResult =
        switch (rewriteResult) {
        | Utils.OutputCode(ocamlCode, _) => Utils.compileOCaml(ocamlCode)
        | Utils.ErrorMessage(_) => defaultResult
        };
      (self.state.reason.result, rewriteResult, ocamlResult);
    };
    let refmtRE2ML = code => {
      let reasonResult = Utils.refmtRE2ML(code);
      let (rewriteResult, ocamlResult) =
        switch (reasonResult) {
        | Utils.OutputCode(jsxv2Code, _) =>
          let (_, rewriteResult, ocamlResult) = rewriteJsxV2(jsxv2Code);
          (rewriteResult, ocamlResult);
        | Utils.ErrorMessage(_) => (defaultResult, defaultResult)
        };
      (code, reasonResult, rewriteResult, ocamlResult);
    };
    let refmtML2RE = code => {
      let (_, rewriteResult, ocamlResult) = rewriteJsxV2(code);
      let reasonResult = Utils.refmtML2RE(code);
      let reasonCode =
        switch (reasonResult) {
        | Utils.OutputCode(code, _) => code
        | Utils.ErrorMessage(_) => self.state.reason.code
        };
      (reasonCode, Utils.OutputCode(code, None), rewriteResult, ocamlResult);
    };
    let onReasonChange = (code, _change) =>
      self.send(UpdateReason(refmtRE2ML(code)));
    let debouncedOnReasonChange = Utils.debounce(onReasonChange, ~wait=250.0);
    let onOCamlJsxChange = (code, _change) =>
      self.send(UpdateOCamlJsx(refmtML2RE(code)));
    let debouncedOnOCamlJsxChange =
      Utils.debounce(onOCamlJsxChange, ~wait=250.0);
    let (jsCode, jsErrorMessage) =
      switch (self.state.ocaml.result) {
      | Utils.OutputCode(jsCode, _) => (jsCode, None)
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
          warnings=?(getWarnings(compilerState.result))
          ?onChange
          readOnly
        />;
      } else {
        ReasonReact.nullElement;
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
        <CodeEditor
          label="JS"
          mode="javascript"
          code=jsCode
          error=?jsErrorMessage
          readOnly=true
        />;
      } else {
        ReasonReact.nullElement;
      };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar
          title="Reason React Playground"
          leftIcon=logo
          onLeftIconClick=(
            _event =>
              Utils.redirect(
                "https://astrada.github.io/reason-react-playground/"
              )
          )
          rightIcon=githubIcon
          onRightIconClick=(
            _event =>
              Utils.openWindow(
                "https://github.com/astrada/reason-react-playground/"
              )
          )>
          <ReactToolbox.IconMenu
            className="white-icon"
            icon=(ReasonReact.stringToElement("more_vert"))
            position=ReactToolbox.IconMenu.Position.TopRight
            menuRipple=true>
            <ReactToolbox.MenuItem
              icon=(ReasonReact.stringToElement("delete"))
              caption="Reset"
              onClick=(_event => self.send(Reset))
            />
          </ReactToolbox.IconMenu>
        </ReactToolbox.AppBar>
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
            onChange=((_checked, _event) => self.send(ToggleJsxV2))
          />
          jsxv2CodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.ocaml.show
            label=(ReasonReact.stringToElement("Show OCaml (read-only)"))
            onChange=((_checked, _event) => self.send(ToggleOCaml))
          />
          ocamlCodeEditor
          <ReactToolbox.Checkbox
            checked=self.state.showJs
            label=(ReasonReact.stringToElement("Show JS (read-only)"))
            onChange=((_checked, _event) => self.send(ToggleJs))
          />
          jsCodeEditor
          <Preview
            code=jsCode
            onDone=(
              error =>
                if (error) {
                  self.send(ShowJs);
                }
            )
          />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>;
  }
};
