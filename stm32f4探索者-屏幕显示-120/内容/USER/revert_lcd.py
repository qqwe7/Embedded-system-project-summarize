import codecs
import re

lcd_path = 'd:/HuaweiMoveData/Users/27907/Desktop/期末大作业_1/期末大作业/HARDWARE/LCD/lcd.c'

try:
    with codecs.open(lcd_path, 'r', 'gbk') as f:
        content = f.read()

    # 1. Revert t1 and j loops
    # matches for(t1=0;t1<200 or for(j=0;j<200
    new_content = re.sub(r'for\s*\(\s*t1\s*=\s*0\s*;\s*t1\s*<\s*200', 'for(t1=0;t1<8', content)
    new_content = re.sub(r'for\s*\(\s*j\s*=\s*0\s*;\s*j\s*<\s*200', 'for(j=0;j<8', new_content)
    
    if new_content != content:
        with codecs.open(lcd_path, 'w', 'gbk') as f:
            f.write(new_content)
        print("Reverted hazardous changes (t1/j loops).")
    else:
        print("No changes to revert (or patterns didn't match).")

    # 2. Search for the font loop again
    # Look for usage of "codeHZ"
    # Print the line containing "codeHZ" and 5 lines before it.
    
    lines = new_content.splitlines()
    for idx, line in enumerate(lines):
        if "codeHZ" in line and "for" in line:
             print(f"Loop on line {idx+1}: {line.strip()}")
        elif "codeHZ" in line:
            # Check previous lines for 'for'
            for offset in range(1, 10):
                if idx - offset >= 0:
                    prev_line = lines[idx-offset]
                    if "for" in prev_line:
                         print(f"Possible loop for {line.strip()} on line {idx-offset+1}: {prev_line.strip()}")
                         break

except Exception as e:
    print(f"Error: {e}")
