/* verilator lint_off UNUSEDSIGNAL */
import "DPI-C" function void ebreak();

module top (
  input clk,
  input rst,
  input [31:0] inst,
  output reg [31:0] pc
);
  
  //control unit用到的线
  wire reg_write, alu_src, mem_write;
  wire [2:0] imm_src, mem_op, wd_src, branch;
  wire [3:0] alu_ctr;
  
  // regfile用到的线
  wire [31:0] rd1, rd2;
  reg [31:0] wd;

  // ImmExt所用的线
  wire [31:0] imm;  // 作为输出的立即数

  // ALU用到的线
  reg [31:0] alu_result;
  wire [31:0] alu_src2;
  wire less, zero;

  // pc相关
  wire [31:0] pc_plus_4, imm_plus_pc, pc_result;
  wire pc_src;

  // DataMem
  wire [31:0] mem_rd;
  
  Control control (
    .op(inst[6:2]),
    .funct3(inst[14:12]),
    .funct7(inst[30]),
    .reg_write(reg_write),
    .imm_src(imm_src),
    .alu_src(alu_src),
    .alu_ctr(alu_ctr),
    .mem_write(mem_write),
    .mem_op(mem_op),
    .wd_src(wd_src),
    .branch(branch)
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

  Alu alu (
    .a(rd1),
    .b(alu_src2),
    .ctr(alu_ctr),
    .y(alu_result),
    .zero(zero),
    .less(less)
  );


  assign pc_plus_4 = 32'b100 + pc;
  assign imm_plus_pc = imm + pc;
  
  DataMem datamem(
    .we(mem_write),
    .ctr(mem_op),
    .addr(alu_result),
    .wd(rd2),
    .rd(mem_rd)
  );

  // 对wd_src的四选一
  always @(*) begin
    casez(wd_src)
      3'b000: wd = alu_result;
      3'b001: wd = pc_plus_4;
      3'b010: wd = imm;
      3'b1??: wd = mem_rd;
    endcase
  end

  PcNext pcnext(
    .branch(branch),
    .zero(zero),
    .less(less),
    .pc_src(pc_src)
  );

  // 对pc_src的三选一
  always @(*) begin
    case(pc_src)
      2'b00: pc_result = pc_plus_4;
      2'b01: pc_result = imm_plus_pc;   // jal
      2'b10: pc_result = alu_result;    // jalr,此时alu_result=rd1 + imm
      default: pc_result = 32'bx;
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

