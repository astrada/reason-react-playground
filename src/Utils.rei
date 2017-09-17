let optionMap: ('a => 'b) => option 'a => option 'b;

type options = {
  leading: bool,
  maxWait: float,
  trailing: bool
};

let defaultOptions: options;

let debounce: 'a => wait::float? => 'a;
