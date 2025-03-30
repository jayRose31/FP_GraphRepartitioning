from pathlib import Path

file_path = Path("third_party/ProcessMappingAnalyzer/build/processmappinganalyzer")  # Relative path
absolute_path = file_path.resolve()
print(absolute_path)
