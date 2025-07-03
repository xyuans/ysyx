module ImmExt(
  input [31:7] inst,
  input [1:0] imm_src,
  output reg [31:0] imm
);
  always@(*) begin
    case(imm_src)
      2'b00: imm = {{20{inst[31]}},inst[31:20]};  // i
      // 2'b01: imm = {{20{inst[31]}},inst[31:25], inst[11:7]};
      2'b01: imm = {inst[31:12], 12'b0};  // u
      2'b10: imm = {{11{inst[31]}}, inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};  // j
      default: imm = {7'b0, inst};
    endcase
  end
endmodule
