# BiQuadFilter
BiQuad IIR Low Pass Filter for Arduino

Filters Arduino data type INT (int16_t).

#### Object creation:
BIQuadFilter filterObject(FreqCutoff, SampleRate);

#### The latest result is returned when adding data:
filteredData = filterObject.in(rawData);

#### The latest result can be accessed without adding data:
filteredData = filterObject.out();



Note: Currently Q is hard coded to 0.7071
