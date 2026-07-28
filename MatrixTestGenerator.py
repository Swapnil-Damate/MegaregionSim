import csv
import math
import random
import os

# Train physics variables
MAX_TRACTIVE_EFFORT = 5000000.0  # 5 Million Newtons
GRAVITY = 9.81
WEIGHT_CLASSES = [10000.0, 50000.0, 150000.0, 300000.0, 500000.0] # kg
GRADIENTS = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0] # degrees (pitch)
WEATHER_COND = ["Clear", "Fog", "Rain", "Snow"]
THROTTLE_NOTCHES = [1, 2, 3, 4, 5, 6, 7, 8]

def run_matrix_test():
    total_permutations = 0
    passed = 0
    failed_stalls = 0
    failed_derails = 0
    
    output_log = "MatrixTestResults.csv"
    
    with open(output_log, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["ScenarioID", "Weight(kg)", "Grade(deg)", "Weather", "Throttle", "TractiveEffort(N)", "GradeResistance(N)", "AdhesionFactor", "Result"])
        
        # We need 5000+ scenarios. We will iterate fine-grained weights and grades.
        fine_weights = [w * 1000 for w in range(10, 510, 5)]  # 100 variations
        fine_grades = [g * 0.1 for g in range(0, 80)] # 80 variations
        
        # 100 * 80 = 8000 scenarios!
        for weight in fine_weights:
            for grade in fine_grades:
                weather = random.choice(WEATHER_COND)
                throttle = 8 # Assuming max throttle for hill climb test
                
                # Math: Grade Resistance = Mass * Gravity * sin(theta)
                grade_rad = math.radians(grade)
                grade_resistance = weight * GRAVITY * math.sin(grade_rad)
                
                tractive_effort = MAX_TRACTIVE_EFFORT * (throttle / 8.0)
                
                # Adhesion is heavily reduced in Rain/Snow
                adhesion_mult = 1.0
                if weather in ["Rain", "Snow"]:
                    adhesion_mult = 0.6 # 40% loss of traction
                
                effective_tractive_effort = tractive_effort * adhesion_mult
                
                if effective_tractive_effort > grade_resistance:
                    result = "PASS"
                    passed += 1
                else:
                    result = "STALL"
                    failed_stalls += 1
                    
                writer.writerow([total_permutations, weight, grade, weather, throttle, effective_tractive_effort, grade_resistance, adhesion_mult, result])
                total_permutations += 1

    print(f"--- MEGAREGION MATRIX TEST COMPLETE ---")
    print(f"Total Scenarios Evaluated: {total_permutations}")
    print(f"Pass (Successfully Climbed): {passed}")
    print(f"Failed (Stalled on Grade): {failed_stalls}")
    print(f"Detailed logs saved to {output_log}")

if __name__ == "__main__":
    run_matrix_test()
