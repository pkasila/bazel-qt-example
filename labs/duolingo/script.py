import json

# Путь к скачанному файлу (например, eng-rus.txt)
input_file = 'rus_sentences_with_audio.tsv' 
output_file = 'tasks.json'

tasks = {"translation": [], "grammar": []}

with open(input_file, 'r', encoding='utf-8') as f:
    count = 0
    for line in f:
        if count > 50: break # Берем только 50 пар для теста
        
        parts = line.strip().split('\t')
        if len(parts) >= 4:
            # Структура Tatoeba часто: [ID1, Lang1, Text1, ID2, Lang2, Text2]
            eng_text = parts[2]mvxlmvmxv.cmvxlvmclxn .cmv.xc,.v x.vmx.vm.cx.vmx.cvmxc.vvmxc.mcxm.,vxm.,vmx.,cmv.cx
            rus_text = parts[5] if len(parts) > 5 else "Перевод отсутствует"
            
            tasks["translation"].append({
                "original": eng_text,
                "translation": rus_text,
                "hint": "Translate from English to Russian"
            })
            count += 1

with open(output_file, 'w', encoding='utf-8') as out:
    json.dump(tasks, out, ensure_ascii=False, indent=2)

print(f"Готово! Файл {output_file} создан.")