type scrollInfo = {
  .
  "left": float,
  "top": float,
  "width": float,
  "height": float,
  "clientWidth": float,
  "clientHeight": float
};

type position = {. "line": float, "ch": float};

type change = {
  .
  "from": position, "_to": position, "text": array(string), "removed": string, "origin": string
};

let make:
  (
    ~autoFocus: bool=?,
    ~autoSave: bool=?,
    ~className: string=?,
    ~defaultValue: string=?,
    ~options: Js.t({..})=?,
    ~onChange: (string, change) => unit=?,
    ~onCursorActivity: (Js.t({..}) => unit)=?,
    ~onFocusChange: (bool => unit)=?,
    ~onScroll: (scrollInfo => unit)=?,
    ~preservePositionScroll: bool=?,
    ~value: string=?,
    array(ReasonReact.reactElement)
  ) =>
  ReasonReact.component(ReasonReact.stateless, ReasonReact.noRetainedProps, ReasonReact.actionless);
