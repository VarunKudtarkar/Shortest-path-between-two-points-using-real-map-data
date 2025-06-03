from geopy.distance import geodesic

# Function to read latitude and longitude from a file
def read_coordinates(file_path):
    coordinates = []
    with open(file_path, 'r') as file:
        for line in file:
            coordinates = line.split(' ', 4)
            coord1 = [coordinates[0], coordinates[1]]
            coord2 = [coordinates[2], coordinates[3]]

            distance = calculate_distance(coord1, coord2)

            write_distance_to_file(distance, 'output.txt')

# Function to calculate distance between two locations
def calculate_distance(coord1, coord2):
    return geodesic(coord1, coord2).meters

def write_distance_to_file(distance, output_file_path):
    with open(output_file_path, 'a') as file:
        file.write(f"{distance:.2f}\n")

# Example usage
if __name__ == "__main__":
    # Read coordinates from file (assuming the format: lat, lon on each line)
    file_path = 'input.txt'
    read_coordinates(file_path)
