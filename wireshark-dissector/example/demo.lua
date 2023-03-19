-- ICMP Dissector script by dtt

local PROTO_NAME = "ICMPvS"
local PROTO_DESC = "ICMP Protocal vS"
local icmp_proto = Proto(PROTO_NAME, PROTO_DESC)

-- 协议字段
icmp_proto.fields = {
    ProtoField.uint8(PROTO_NAME..   ".type",        "TYPE"),
    ProtoField.uint8(PROTO_NAME..   ".code",        "CODE"),
    ProtoField.uint16(PROTO_NAME..  ".checksum",    "CHECKSUM"),
    ProtoField.uint16(PROTO_NAME..  ".id",          "ID"),
    ProtoField.uint16(PROTO_NAME..  ".seq",         "SEQ"),
    ProtoField.bytes(PROTO_NAME..   ".data",        "DATA")
}

-- 协议解析
function icmp_proto.dissector(tvb, pinfo, treeitem)
    -- 设置一些 UI 上面的信息
    pinfo.cols.protocol:set(PROTO_NAME)
    pinfo.cols.info:set(PROTO_DESC)

    local tvb_len = tvb:len()

    -- 在上一级解析树上创建 foo 的根节点
    local icmp_tree = treeitem:add(icmp_proto, tvb:range(tvb_len))
    icmp_tree:add(icmp_proto.fields[1], tvb(0, 1))     --表示从0开始1个字节
    icmp_tree:add(icmp_proto.fields[2], tvb(1, 1))
    icmp_tree:add(icmp_proto.fields[3], tvb(2, 2))
    icmp_tree:add(icmp_proto.fields[4], tvb(4, 2))
    icmp_tree:add(icmp_proto.fields[5], tvb(6, 2))
    icmp_tree:add(icmp_proto.fields[6], tvb(8, tvb_len-8))
end

-- 注册解析
DissectorTable.get("ip.proto"):add(1, icmp_proto)
