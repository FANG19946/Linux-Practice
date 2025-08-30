def convert_length(value, from_unit, to_unit):
    units_in_meters = {"m": 1, "cm": 0.01, "km": 1000, "inch": 0.0254, "ft": 0.3048}
    if from_unit not in units_in_meters or to_unit not in units_in_meters:
        return None
    return value * units_in_meters[from_unit] / units_in_meters[to_unit]

def convert_weight(value, from_unit, to_unit):
    units_in_kg = {"kg": 1, "g": 0.001, "lb": 0.453592, "oz": 0.0283495}
    if from_unit not in units_in_kg or to_unit not in units_in_kg:
        return None
    return value * units_in_kg[from_unit] / units_in_kg[to_unit]

def convert_temperature(value, from_unit, to_unit):
    if from_unit == "C":
        if to_unit == "F":
            return (value * 9/5) + 32
        elif to_unit == "K":
            return value + 273.15
    elif from_unit == "F":
        if to_unit == "C":
            return (value - 32) * 5/9
        elif to_unit == "K":
            return (value - 32) * 5/9 + 273.15
    elif from_unit == "K":
        if to_unit == "C":
            return value - 273.15
        elif to_unit == "F":
            return (value - 273.15) * 9/5 + 32
    return value

def main():
    print("Welcome to the Unit Converter!")
    category = input("Choose category (length/weight/temperature): ").lower()
    value = float(input("Enter value: "))
    from_unit = input("From unit: ").lower()
    to_unit = input("To unit: ").lower()

    if category == "length":
        result = convert_length(value, from_unit, to_unit)
    elif category == "weight":
        result = convert_weight(value, from_unit, to_unit)
    elif category == "temperature":
        result = convert_temperature(value, from_unit, to_unit)
    else:
        result = None

    if result is not None:
        print(f"{value} {from_unit} = {result} {to_unit}")
    else:
        print("Invalid units or category!")

if __name__ == "__main__":
    main()
