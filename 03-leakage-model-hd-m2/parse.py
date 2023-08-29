import os

def process_file(filepath):
    with open(filepath, 'r') as file:
        lines = file.readlines()
        
    with open(filepath, 'w') as file:
        for line in lines:
            columns = line.split()
            if len(columns) == 2:
                try:
                    col1 = int(float(columns[0]))
                    col2_str = columns[1].rstrip('u')
                    if col2_str == 'nan':
                        col2_str = 0
                    col2 = int(float(col2_str))
                    file.write(f"{col1} {col2}\n")
                except ValueError:
                    file.write(line)
            else:
                file.write(line)

def main():
    directory = 'data/out-0827-1837/out-0827-1837/'
    for filename in os.listdir(directory):
        if filename.endswith('.out'):
            process_file(os.path.join(directory, filename))

if __name__ == "__main__":
    main()