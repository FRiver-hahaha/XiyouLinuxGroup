# 支付宝红包口令完整解码器

import base64
import binascii

# 原始16进制字符串
hex_str = "584855324E6A6468584855354D47466C584855324E6A5A6C584855314D6A4669584855314E546C6A58485532596A4979584855324D474534584855324E7A5931584855334F54566B584855324D474534584855324E574977584855315A546330584855315A6D5669584855305A545577"

print("=" * 60)
print("支付宝红包口令解码器")
print("=" * 60)

# 第一步：16进制转ASCII
try:
    ascii_text = binascii.unhexlify(hex_str).decode('utf-8')
    print(f"\n第一步 (16进制转ASCII):")
    print(ascii_text)
except:
    print("16进制转换失败")

# 第二步：观察发现这是多层base64编码
print("\n" + "=" * 60)
print("开始多层Base64解码...")
print("=" * 60)

current = ascii_text
layer = 1

while True:
    try:
        # 尝试Base64解码
        decoded = base64.b64decode(current).decode('utf-8')
        print(f"\n第{layer}层Base64解码后:")
        print(decoded)
        current = decoded
        layer += 1
        
        # 如果解码结果看起来像最终结果（包含中文字符），就停止
        if any('\u4e00' <= ch <= '\u9fff' for ch in decoded):
            print("\n" + "=" * 60)
            print("最终解码结果（支付宝红包口令）:")
            print("=" * 60)
            print(decoded)
            break
            
    except Exception as e:
        print(f"\n解码完成，最终结果:")
        print(current)
        break

print("\n提示: 如果是支付宝口令，可以直接在支付宝搜索框输入领取红包")