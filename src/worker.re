type t;

external make : string => t = "Worker" [@@bs.new];

external importScripts : t => string => unit = "" [@@bs.send];

external postMessage : t => 'a => unit = "" [@@bs.send];

external onMessage : t => (Js.t {. data : 'a} => unit) => unit = "onmessage" [@@bs.set];

module Script = {
  external self : t = "" [@@bs.val];
};
