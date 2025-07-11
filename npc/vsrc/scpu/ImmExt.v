module ImmExt(
  input [31:7] inst,
  input [2:0] imm_src,
  output reg [31:0] imm
);
  always@(*) begin
    case(imm_src)
      3'b000: imm = {{20{inst[31]}}, inst[31:20]};  // i
      3'b001: imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};  // s
      3'b010: imm = {{19{inst[31]}}, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0}; // b
      3'b011: imm = {inst[31:12], 12'b0};  // u
      3'b100: imm = {{11{inst[31]}}, inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};  // j
      default: imm = {7'b0, inst};
    endcase
  end
endmodule
