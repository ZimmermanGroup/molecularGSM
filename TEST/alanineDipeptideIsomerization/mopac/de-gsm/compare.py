import csv

def convertToFloat(inList):
    for element in inList:
        try:
            yield float(element)
        except ValueError:
            yield element

def subtractFloat(number1, number2):
    try:
        return (number1 - number2)
    except TypeError:
        pass

def main():
    standard = csv.reader(open('stringfile.standard', 'r'), delimiter=' ')
    currentOutput = csv.reader(open('stringfile.xyz0001', 'r'), delimiter=' ')

    for rowStd, rowOut in zip(standard, currentOutput):
        rowStd = filter(None, rowStd)
        rowOut = filter(None, rowOut)
        for valStd, valOut in zip(list(convertToFloat(rowStd)), list(convertToFloat(rowOut))):
            if ((subtractFloat(valStd, valOut)) > 0.001):
                print ((subtractFloat(valStd, valOut)))
                return 2

if __name__ == "__main__":
    main()
