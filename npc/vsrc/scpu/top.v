/* verilator lint_off UNUSEDSIGNAL */
import "DPI-C" function void ebreak();

module top (
  input clk,
  input rst,
  input [31:0] inst,
  output reg [31:0] pc
);
  wire [31:0] rd1, rd2, wd;
  wire [31:0] pc_plus_4, alu_result,  imm_plus_pc, alu_src2, imm;
  reg [31:0] pc_result;
  // 解码用的线,控制
  wire reg_write, alu_src;
  wire [1:0] imm_src, wd_src, pc_src;
  
  Control control (
    .op(inst[6:0]),
    .reg_write(reg_write),
    .imm_src(imm_src),
    .alu_src(alu_src),
    .wd_src(wd_src),
    .pc_src(pc_src)
  );

  RegFile #(.ADDR_WIDTH(5), .DATA_WIDTH(32)) rf (
    .clk(clk),
    .we(reg_write),
    .ra1(inst[19:15]),
    .ra2(inst[24:20]),
    .rd1(rd1),
    .rd2(rd2),
    .wa(inst[11:7]),
    .wd(wd)
  );

  ImmExt immext(
    .inst(inst[31:7]),
    .imm_src(imm_src),
    .imm(imm)
  );
  
  // 对alu_src2的二选一
  Mux21 mux21 (
    .a(rd2),  // rd2
    .b(imm),  // imm 立即数
    .s(alu_src),
    .y(alu_src2)
  );

  Alu #(32) alu (
    .a(rd1),
    .b(alu_src2),
    .y(alu_result)
  );


  assign pc_plus_4 = 32'b100 + pc;
  assign imm_plus_pc = imm + pc;
  
  // 对wd_src的四选一
  Mux41 mux41 (
    .a1(alu_result),   // add, addi
    .a2(pc_plus_4),          // lui
    .a3(imm),  // auipc
    .a4(imm+pc),    // jal, jalr
    .s(wd_src),
    .y(wd)
  );
  
  // 对pc_src的三选一
  always @(*) begin
    case(pc_src)
      2'b00: pc_result = pc_plus_4;
      2'b01: pc_result = imm_plus_pc;   // jal
      2'b10: pc_result = alu_result;    // jalr,此时alu_result=rd1 + imm
      default: pc_result = pc_plus_4;
    endcase
  end

  always @(posedge clk) begin
    if (rst) pc <= 32'h80000000;
    else begin
      if (inst[6:0] == 7'b1110011) ebreak();
      pc <= pc_result;
    end
  end

endmodule

