let errorBodyStyle = ReactDOMRe.Style.make(~fontFamily="monospace", ~fontSize="12pt", ());

let warningStyle = ReactDOMRe.Style.make(~backgroundColor="#fff8dc", ~padding="10px 20px", ());

let component = ReasonReact.statelessComponent("Warnings");

let make = (~warnings=?, _children) => {
  ...component,
  render: (_self) =>
    switch warnings {
    | None => ReasonReact.nullElement
    | Some(w) =>
      <div style=warningStyle>
        <div style=errorBodyStyle> (ReasonReact.stringToElement(w)) </div>
      </div>
    }
};
