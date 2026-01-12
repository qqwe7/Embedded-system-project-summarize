import codecs

main_path = 'd:/HuaweiMoveData/Users/27907/Desktop/期末大作业_1/期末大作业/USER/main.c'

try:
    with codecs.open(main_path, 'r', 'gbk') as f:
        lines = f.readlines()
    
    new_lines = []
    skip_next = False
    i = 0
    while i < len(lines):
        line = lines[i]
        # 跳过学院相关的代码块
        if '学院' in line and 'sprintf' in line:
            # 跳过这行和下一行 (Show_Str)
            i += 2
            print("删除了学院行")
            continue
        # 调整其他信息的Y坐标（从90开始，依次70, 90, 110）
        # 班级：原90 -> 70
        # 姓名：原110 -> 90  
        # 学号：原130 -> 110
        if 'Show_Str(30, 90' in line and '班级' not in line:
            line = line.replace('Show_Str(30, 90', 'Show_Str(30, 70')
        elif 'Show_Str(30, 110' in line:
            line = line.replace('Show_Str(30, 110', 'Show_Str(30, 90')
        elif 'Show_Str(30, 130' in line:
            line = line.replace('Show_Str(30, 130', 'Show_Str(30, 110')
        new_lines.append(line)
        i += 1
    
    with codecs.open(main_path, 'w', 'gbk') as f:
        f.writelines(new_lines)
    
    print("成功删除学院行并调整布局")

except Exception as e:
    print(f"错误: {e}")
