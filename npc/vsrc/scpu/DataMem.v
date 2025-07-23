import "DPI-C" function int pmem_read(input int unsigned raddr);
import "DPI-C" function void pmem_write(input int unsigned waddr, input int unsigned wdata, input int len);

/* verilator lint_off BLKSEQ */
module DataMem (
  input clk,
  input we,
  input [2:0] ctr,
  input [31:0] addr,
  input [31:0] wd,
  output reg [31:0] rd
);
  // mem_op: 000-1byte,sign  001-2bs  010-4b  100-1bu  101-2bu
  reg [31:0] read_data;
  
//   always @(posedge clk) begin
//     read_data = pmem_read(addr);
//     // 写操作：仅在写使能时执行
//     if (we) begin
//       case(ctr)
//         3'b000, 3'b100: pmem_write(addr, wd, 1);
//         3'b001, 3'b101: pmem_write(addr, wd, 2);
//         3'b010:        pmem_write(addr, wd, 4);
//         default: ; // 忽略非法操作
//       endcase
//
//     end
//
//     // 第二阶段：数据格式转换
//     case(ctr)
//       3'b000: rd <= {{24{read_data[7]}},  read_data[7:0]};
//       3'b001: rd <= {{16{read_data[15]}}, read_data[15:0]};
//       3'b010: rd <= read_data;
//       3'b100: rd <= {24'b0, read_data[7:0]};
//       3'b101: rd <= {16'b0, read_data[15:0]};
//       default: rd <= 32'bx;
//     endcase 
//   end
// endmodule
  always @(posedge clk) begin
    read_data = pmem_read(addr);
    case (ctr)
      3'b000: begin 
        rd = {{24{read_data[7] }}, read_data[7:0] }; 
        if (we) begin
          pmem_write(addr, wd, 1);
        end
      end
      3'b001: begin 
        rd = {{16{read_data[15]}}, read_data[15:0]};
        if (we) begin
          pmem_write(addr, wd, 2);
        end
      end
      3'b010: begin
        rd = read_data;
        if (we) begin
          pmem_write(addr, wd, 4);
        end
      end
      3'b100: begin
        rd = {24'b0, read_data[7:0]};
        if (we) begin
          pmem_write(addr, wd, 1);
        end
      end
      3'b101: begin
        rd = {16'b0, read_data[15:0]};
        if (we) begin
          pmem_write(addr, wd, 2);
        end
      end
      default: rd = 32'bx;
    endcase

  end
endmodule
