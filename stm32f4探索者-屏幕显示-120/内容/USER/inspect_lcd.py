import codecs

lcd_path = 'd:/HuaweiMoveData/Users/27907/Desktop/期末大作业_1/期末大作业/HARDWARE/LCD/lcd.c'

try:
    with codecs.open(lcd_path, 'r', 'gbk') as f:
        content = f.read()

    # Search for "codeHZ"
    idx = content.find("codeHZ")
    if idx != -1:
        print(f"Found 'codeHZ' at index {idx}")
        # Print -200 to +200 chars
        start = max(0, idx - 200)
        end = min(len(content), idx + 200)
        print("CONTEXT:")
        print(content[start:end])
    else:
        print("Could not find 'codeHZ' in lcd.c")

except Exception as e:
    print(f"Error: {e}")
