--[[
    创建一个新的协议结构 foo_proto
    第一个参数是协议名称会体现在过滤器中
    第二个参数是协议的描述信息，无关紧要
--]]

local PROTO_NAME = "ICMPvS"
local PROTO_DESC = "ICMP Protocal vS"
local icmp_proto = Proto(PROTO_NAME, PROTO_DESC)

--[[
    下面定义字段
--]]
local icmp_type = ProtoField.uint8(PROTO_NAME..".type", "TYPE", base.DEC)
local icmp_code = ProtoField.uint8(PROTO_NAME..".code", "CODE", base.DEC)
local icmp_checksum = ProtoField.uint16(PROTO_NAME..".checksum", "CHECKSUM", base.DEC)
local icmp_id = ProtoField.uint16(PROTO_NAME..".id", "ID", base.DEC)
local icmp_seq = ProtoField.uint16(PROTO_NAME..".seq", "SEQ", base.DEC)
local icmp_data = ProtoField.bytes(PROTO_NAME..".data", "DEATA")

-- 将字段添加都协议中
icmp_proto.fields = {
    icmp_type,
    icmp_code,
    icmp_checksum,
    icmp_id,
    icmp_seq,
    icmp_data
}
--]]

--[[
    下面定义 foo 解析器的主函数，这个函数由 wireshark调用
    第一个参数是 Tvb 类型，表示的是需要此解析器解析的数据
    第二个参数是 Pinfo 类型，是协议解析树上的信息，包括 UI 上的显示
    第三个参数是 TreeItem 类型，表示上一级解析树
--]]
function icmp_proto.dissector(tvb, pinfo, treeitem)
    -- 设置一些 UI 上面的信息
    pinfo.cols.protocol:set(PROTO_NAME)
    pinfo.cols.info:set(PROTO_DESC)

    local tvb_len = tvb:len()

    -- 在上一级解析树上创建 foo 的根节点
    local icmp_tree = treeitem:add(icmp_proto, tvb:range(tvb_len))
    icmp_tree:add(icmp_type, tvb(0, 1))     --表示从0开始1个字节
    icmp_tree:add(icmp_code, tvb(1, 1))
    icmp_tree:add(icmp_checksum, tvb(2, 2))
    icmp_tree:add(icmp_id, tvb(4, 2))
    icmp_tree:add(icmp_seq, tvb(6, 2))
    icmp_tree:add(icmp_data, tvb(8, tvb_len-8))
end

-- 向 wireshark 注册协议插件被调用的条件
local icmp_table = DissectorTable.get("ip.proto")
icmp_table:add(1, icmp_proto)
