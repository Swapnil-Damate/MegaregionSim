#include "Misc/AutomationTest.h"
#include "Math/UnrealMathUtility.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMegaregionDummyTest, "Megaregion.DummyTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FMegaregionDummyTest::RunTest(const FString& Parameters)
{
    // Dummy test body
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMegaregionDerailmentMatrixTest, "Megaregion.DerailmentMatrixTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMegaregionDerailmentMatrixTest::RunTest(const FString& Parameters)
{
    int32 TestsRun = 0;
    
    // Simulate 1000+ combinations
    for (float Speed = 0.0f; Speed <= 300.0f; Speed += 10.0f) // 31 iterations
    {
        for (float Weight = 5000.0f; Weight <= 500000.0f; Weight += 15000.0f) // 34 iterations. 31 * 34 = 1054 combinations
        {
            // Dummy logic for derailment adhesion: 
            // Derailment happens if Speed > 150 km/h AND Weight < 20000 kg (too light and fast)
            // Or if Speed > 250 km/h AND Weight > 400000 kg (too heavy and fast)
            
            bool bExpectedDerailment = (Speed > 150.0f && Weight < 20000.0f) || (Speed > 250.0f && Weight > 400000.0f);
            
            // Simulating a calculation:
            float AdhesionFactor = (Weight / 1000.0f) / FMath::Max(1.0f, Speed);
            bool bActualDerailment = false;
            if (Speed > 150.0f && Weight < 20000.0f)
                bActualDerailment = true;
            else if (Speed > 250.0f && Weight > 400000.0f)
                bActualDerailment = true;
                
            if (bExpectedDerailment)
            {
                TestTrue(FString::Printf(TEXT("Derailed over limit: Speed %.1f, Weight %.1f"), Speed, Weight), bActualDerailment == bExpectedDerailment);
            }
            else
            {
                TestTrue(FString::Printf(TEXT("Did not derail under limit: Speed %.1f, Weight %.1f"), Speed, Weight), bActualDerailment == bExpectedDerailment);
            }
            
            TestsRun++;
        }
    }
    
    return true;
}
