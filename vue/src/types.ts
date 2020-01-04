type AutomaticSwitching = {
    state: boolean;
    start: string;
    end: string;
    days: string[];
    sD:number;
    eD:number;
    sT: string;
    eT: string;
    onrange: number[];
    offrange: number[];
    irreg: boolean;
    mode: number;
};

type SingleConfig = {
    name: string;
    state: boolean;
    on: RadioSignal;
    off: RadioSignal;
    auto: AutomaticSwitching[];
};

type RadioSignal = {
    protocol: number;
    length: number;
    value: number;
};

type AllConfigs = SingleConfig[];


type SingleDevice = {
    state: string;
};




interface TabHandleData {
    internalLazyValue: number;
  }
  interface TabHandle {
    _data: TabHandleData;
    $children: any[];
  }
