let component = ReasonReact.statelessComponent("CodeEditor");

let rowStyle =
  ReactDOMRe.Style.make(
    ~flex="1",
    ~minHeight="0",
    ~border="1px solid #aaa",
    ~position="relative",
    ~overflow="auto",
    ~margin="20px 0 20px 0",
    ()
  );

let labelStyle =
  ReactDOMRe.Style.make(
    ~position="absolute",
    ~top="0",
    ~right="0",
    ~backgroundColor="rgba(200, 200, 200, 0.6)",
    ~padding="5px 10px",
    ~fontWeight="bold",
    ~fontFamily="monospace",
    ~color="black",
    ~fontSize="14pt",
    ~lineHeight="14px",
    ~flexDirection="row",
    ~alignItems="center",
    ~zIndex="20",
    ~borderRadius="0 0 0 5px",
    ()
  );

let loadingMaskStyle =
  ReactDOMRe.Style.make(
    ~position="absolute",
    ~top="0",
    ~left="0",
    ~backgroundColor="rgba(200, 200, 200, 0.6)",
    ~padding="5px 10px",
    ~fontWeight="bold",
    ~fontFamily="monospace",
    ~color="black",
    ~fontSize="14pt",
    ~lineHeight="14px",
    ~flexDirection="row",
    ~alignItems="center",
    ~zIndex="20",
    ()
  );

let make =
    (
      ~label,
      ~mode,
      ~loading=false,
      ~code=?,
      ~error=?,
      ~warnings=?,
      ~onChange=?,
      ~readOnly=?,
      _children
    ) => {
  ...component,
  render: _self => {
    let options = {"lineNumbers": Js.Boolean.to_js_boolean(true)};
    let options = Js.Obj.assign(options, {"mode": mode});
    let options =
      switch (readOnly) {
      | Some(ro) =>
        Js.Obj.assign(options, {"readOnly": Js.Boolean.to_js_boolean(ro)})
      | None => options
      };
    let loadingMask =
      if (loading) {
        <div style=loadingMaskStyle>
          (ReasonReact.stringToElement("Loading..."))
        </div>;
      } else {
        ReasonReact.nullElement;
      };
    <div style=rowStyle>
      loadingMask
      <div style=labelStyle> (ReasonReact.stringToElement(label)) </div>
      <CodeMirror value=?code ?onChange options preservePositionScroll=true />
      <Error errorMessage=?error />
      <Warnings ?warnings />
    </div>;
  }
};
