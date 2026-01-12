import codecs
import re

lcd_path = 'd:/HuaweiMoveData/Users/27907/Desktop/期末大作业_1/期末大作业/HARDWARE/LCD/lcd.c'

try:
    with codecs.open(lcd_path, 'r', 'gbk') as f:
        content = f.read()

    # Locate Show_Str function
    show_str_idx = content.find("Show_Str")
    if show_str_idx == -1:
        print("Error: Could not find Show_Str function.")
        exit(1)
        
    # Search for the loop AFTER Show_Str
    # Pattern: for(...) { if... codeHZ_16 }
    # Try to find "codeHZ_16"
    codehz_idx = content.find("codeHZ_16", show_str_idx)
    if codehz_idx == -1:
        print("Error: Could not find 'codeHZ_16' usage.")
        exit(1)
        
    # Search BACKWARDS from codeHZ_16 to find the 'for' loop
    # We look for "for"
    for_loop_idx = content.rfind("for", show_str_idx, codehz_idx)
    if for_loop_idx == -1:
        print("Error: Could not find 'for' loop before 'codeHZ_16'.")
        exit(1)
        
    loop_str = content[for_loop_idx:codehz_idx]
    print(f"Found loop candidate: {loop_str[:50]}...")
    
    # Replace the condition in this specific loop string
    # We look for "cn<" followed by digits
    new_loop_str = re.sub(r'(cn\s*<\s*)(\d+)', r'\1 200', loop_str)
    
    if new_loop_str != loop_str:
        # Construct new content
        new_content = content[:for_loop_idx] + new_loop_str + content[codehz_idx:]
        with codecs.open(lcd_path, 'w', 'gbk') as f:
            f.write(new_content)
        print("Successfully updated loop limit to 200.")
    else:
        print("Could not regex match loop condition.")

except Exception as e:
    print(f"Error: {e}")
