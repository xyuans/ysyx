module Mux41(
  input [31:0] a1,
  input [31:0] a2,
  input [31:0] a3,
  input [31:0] a4,
  input [1:0] s,
  output [31:0] y
);
  MuxKey #(4, 2, 32) i0 (y, s, {
    2'b00, a1,
    2'b01, a2,
    2'b10, a3,
    2'b11, a4
  });
endmodule
