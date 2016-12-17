# for reading csv format files
import csv

# function to convert strings to float and skip conversion if the value is not a float
def convertToFloat(inList):
    for element in inList:
        try:
            yield float(element)
        except ValueError:
            yield element

# subtract two floats and skip if string
def subtractFloat(number1, number2):
    try:
        return (number1 - number2)
    except TypeError:
        pass

def main():
    threshold = 0.001
    try:
        # read standard and output files
        standard = csv.reader(open('stringfile.standard', 'r'), delimiter=' ')
        currentOutput = csv.reader(open('stringfile.xyz0001', 'r'), delimiter=' ')
    # error if file does not exist
    except IOError:
        print("Error: File cannot be found!")
        exit(1)
    # loop over elements of two files simultaneously 
    for rowStd, rowOut in zip(standard, currentOutput):
        rowStd = filter(None, rowStd)
        rowOut = filter(None, rowOut)
        for valStd, valOut in zip(list(convertToFloat(rowStd)), list(convertToFloat(rowOut))):
            # error if difference larger than threshold
            if ((subtractFloat(valStd, valOut)) > threshold):
                print ((subtractFloat(valStd, valOut)))
                exit(2)
    return 0

if __name__ == "__main__":
    main()
