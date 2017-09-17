type scrollInfo =
  Js.t {
    .
    left : float,
    top : float,
    width : float,
    height : float,
    clientWidth : float,
    clientHeight : float
  };

type position = Js.t {. line : float, ch : float};

type change =
  Js.t {. from : position, _to : position, text : array string, removed : string, origin : string};

let make:
  autoFocus::bool? =>
  autoSave::bool? =>
  className::string? =>
  defaultValue::string? =>
  options::Js.t {..}? =>
  onChange::(string => change => unit)? =>
  onCursorActivity::ReasonReact.Callback.t (Js.t {..})? =>
  onFocusChange::ReasonReact.Callback.t Js.boolean? =>
  onScroll::ReasonReact.Callback.t scrollInfo? =>
  preservePositionScroll::bool? =>
  value::string? =>
  array ReasonReact.reactElement =>
  ReasonReact.component ReasonReact.stateless ReasonReact.noRetainedProps ReasonReact.actionless;
