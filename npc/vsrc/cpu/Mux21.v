module Mux21(
  input [31:0] a,
  input [31:0] b,
  input s,
  output [31:0] y
);
  // 输入信号的数目， 选择信号的宽度， 数据的宽度
  MuxKey #(2, 1, 32) i0 (y, s, {
    1'b0, a,
    1'b1, b
  });
endmodule
