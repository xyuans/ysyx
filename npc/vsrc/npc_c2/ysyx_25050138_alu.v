module alu(
	input [31:0] a,
	input [31:0] b,
	input [2:0] control,
	output reg [31:0] result,
	output zero
);

  wire [31:0] sum;
  wire [31:0] control0_extend = {32{control[0]}};
	assign {carry, sum} = a + (32{control[0]} ^ b) + control[0];  // 减法时b取反再加1
  assign zero = ~(|result);
  /*1.执行加减法或比较运算 2.a与结果符号相反 3.加法时ab符号相同，减法时ab符号相反*/
  assign overflow = ~(control[1]^control[2]) & (a[3]^sum[3]) & ~(control[0]^a[3]^b[3]); 
  //assign overflow = (a[3] == t_no_cin[3])&&(carry != a[3]);
	
  wire less;
  assign less = sum[3] ^ overflow;

  wire equal;
  assign equal = zero & ~overflow;

	always@(*) begin
		case(control)
			3'b000: result = sum;
			3'b001: result = sum;
			3'b010: result = ~a;
			3'b011: result = a & b;
			3'b100: result = a | b;
			3'b101: result = a ^ b;
			3'b110: result = {3'b000, less};
			3'b111: result = {3'b000, equal};
		endcase
	end
endmodule
