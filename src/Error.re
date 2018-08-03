let errorBodyStyle =
  ReactDOMRe.Style.make(~fontFamily="monospace", ~fontSize="12pt", ());

let errorStyle =
  ReactDOMRe.Style.make(~backgroundColor="#faa", ~padding="10px 20px", ());

let component = ReasonReact.statelessComponent("Error");

let make = (~errorMessage=?, _children) => {
  ...component,
  render: _self =>
    switch (errorMessage) {
    | None => ReasonReact.null
    | Some(e) =>
      <div style=errorStyle>
        <div style=errorBodyStyle> (ReasonReact.string(e)) </div>
      </div>
    }
};
