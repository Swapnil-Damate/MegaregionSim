import json
import os

transcript_path = "/Users/swapnilanildamate/.gemini/antigravity/brain/81d1683c-4a96-4686-869d-d35403caf18b/.system_generated/logs/transcript.jsonl"
output_path = "ChatHistory.md"

with open(output_path, "w", encoding="utf-8") as out_f:
    out_f.write("# MegaregionSim AI Chat History\n\n")
    
    if not os.path.exists(transcript_path):
        out_f.write("Transcript file not found.\n")
    else:
        with open(transcript_path, "r", encoding="utf-8") as in_f:
            for line in in_f:
                try:
                    data = json.loads(line)
                    step_type = data.get("type", "")
                    content = data.get("content", "")
                    
                    if not content:
                        continue
                        
                    if step_type == "USER_INPUT":
                        out_f.write("## USER:\n")
                        out_f.write(content + "\n\n")
                        out_f.write("---\n\n")
                    elif step_type == "PLANNER_RESPONSE" or step_type == "MODEL":
                        out_f.write("## AI:\n")
                        out_f.write(content + "\n\n")
                        out_f.write("---\n\n")
                except Exception as e:
                    pass

print("Chat history exported to ChatHistory.md")
