[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

type jsCompilerResult =
  | JsCode string
  | JsErrorMessage string;

type state = {
  reasonCode: string,
  jsCompilerResult,
  compiling: bool
};

type action =
  | UpdateJsCompilerResult jsCompilerResult
  | Compile string;

let component = ReasonReact.reducerComponent "App";

let make _children => {
  ...component,
  initialState: fun () => {reasonCode: "", jsCompilerResult: JsCode "", compiling: false},
  reducer: fun action state =>
    switch action {
    | UpdateJsCompilerResult jsCompilerResult =>
      ReasonReact.Update {...state, compiling: false, jsCompilerResult}
    | Compile reasonCode => ReasonReact.Update {...state, reasonCode, compiling: true}
    },
  render: fun self => {
    let logo = <Logo />;
    let compile code event => {
      let worker = Worker.make "CompilerWorker.js";
      Worker.onMessage
        worker
        (
          fun messageEvent => {
            let result = CompilerWorker.toCompilerResult messageEvent##data;
            let jsCode = Js.Nullable.to_opt result##js_code;
            let jsErrorMessage =
              Js.Option.firstSome
                (Js.Nullable.to_opt result##js_error_msg) (Js.Nullable.to_opt result##message)
              |> Js.Option.getWithDefault "";
            let jsCompilerResult =
              if (Js.Option.isSome jsCode) {
                JsCode (Js.Option.getExn jsCode)
              } else {
                JsErrorMessage jsErrorMessage
              };
            let reduce = self.reduce (fun _event => UpdateJsCompilerResult jsCompilerResult);
            reduce event
          }
        );
      Worker.postMessage worker code;
      let reduce = self.reduce (fun _event => Compile code);
      reduce event
    };
    let (jsCode, jsErrorMessage) =
      switch self.state.jsCompilerResult {
      | JsCode jsCode => (jsCode, "")
      | JsErrorMessage jsErrorMessage => ("", jsErrorMessage)
      };
    let onChange code _change => compile code ();
    let debouncedOnChange = Utils.debounce onChange wait::500.0;
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar title="App example" leftIcon=logo />
        <section>
          <CodeEditor label="Reason" code=self.state.reasonCode onChange=debouncedOnChange />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=(compile self.state.reasonCode)
            disabled=self.state.compiling
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
