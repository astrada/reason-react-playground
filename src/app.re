[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

type state = {reasonCode: string};

type action =
  | UpdateReasonCode string
  | Compile;

let component = ReasonReact.reducerComponent "App";

let make _children => {
  ...component,
  initialState: fun () => {reasonCode: ""},
  reducer: fun action state =>
    switch action {
    | UpdateReasonCode code => ReasonReact.Update {...state, reasonCode: code}
    | Compile => ReasonReact.NoUpdate
    },
  render: fun self => {
    let logo = <Logo />;
    let onChange code event => {
      let reduce = self.reduce (fun _event => UpdateReasonCode code);
      reduce event
    };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar title="App example" leftIcon=logo />
        <section style=(ReactDOMRe.Style.make padding::"20px" ())>
          <ReactToolbox.Input
            _type="text"
            multiline=true
            label=(ReasonReact.stringToElement "ReasonML")
            value=self.state.reasonCode
            onChange
          />
          <ReactToolbox.Button label="Compile" primary=true />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
