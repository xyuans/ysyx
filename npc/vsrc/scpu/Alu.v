
/*
* 0_001-sll  0_101-srl  1_101-srr
* 移位器需要对这几种情况选择
* 
 */
// ctr[3]-sub/add or logicl/arith  ctr[0]-sign/unsign
// ctr[2:0] 000-sum 
module Alu (
  input [31:0] a,
  input [31:0] b,
  input [3:0] ctr,
  output reg [31:0] y,
  output zero,  // 比较是否相等所用
  output less   // 比较大小关系时所用
);
  
  wire [31:0] sum;
  wire carry;
  wire s_overflow;  // less信号所用

  reg [31:0] shift;
  
  wire is_sub = ctr[3];  // 内部解码
  // carry可zuo无符号减法溢出判断.未溢出说明a-b够减，即a>b。
  // carry为0,溢出，说明a>b。
  // carry为1,未溢出，说明a<b
	assign {carry, sum} = a + ({32{is_sub}} ^ b) + is_sub;  // 减法时b取反再加1
  
  // 有符号减法溢出判断
  // 1.a与结果符号相反 2.减法时ab符号相反
  assign s_overflow = (a[3]^sum[3]) & (a[3]^b[3]);
  

  /*无论是有符号还是无符号，做减法运算，等于零时一定不会溢出，所以zero无需考虑是否溢出*/
  // ctr[0]=1代表无符号数
  assign zero = ~(|sum);
  assign less = ctr[0] ? carry : (s_overflow ^ sum[31]);
  
  
  
  always @(*) begin
    case(ctr[3:2])
      2'b00: shift = a << b[4:0];
      2'b01: shift = a >> b[4:0];
      2'b11: shift = a >>> b[4:0];
      default: shift = 32'bx;
    endcase
  end

  always @(*) begin
    case (ctr[2:0])
      3'b000: y = sum;            // add,sub
      3'b001: y = shift;          // sll
      3'b010: y = {32{less}};     // slt
      3'b011: y = {32{less}};          // sltu
      3'b100: y = a ^ b;
      3'b101: y = shift;  // srl, sra
      3'b110: y = a | b;
      3'b111: y = a & b;
    endcase
  end
endmodule
