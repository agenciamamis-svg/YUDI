# 🔌 Mapa de pines — YUDI (Arduino UNO)

| Componente / Acción | Pin Arduino | Tipo | Descripción |
|----------------------|-------------|------|--------------|
| MOLESTO             | D2          | Digital | Botón emoción “Molesto” |
| FELIZ               | D3          | Digital | Botón emoción “Feliz” |
| TRISTE              | D4          | Digital | Botón emoción “Triste” |
| HOLA                | D5          | Digital | Botón cortesía “Hola” |
| GRACIAS             | D6          | Digital | Botón cortesía “Gracias” |
| POR FAVOR           | D7          | Digital | Botón cortesía “Por favor” |
| DE NADA             | D8          | Digital | Botón cortesía “De nada” |
| HASTA LUEGO         | D9          | Digital | Botón cortesía “Hasta luego” |
| MODO EMOCIONES      | D10         | Digital | Selecciona modo emociones |
| MODO CORTESÍA       | D11         | Digital | Selecciona modo cortesía |
| SÍ (Confirmar)      | A0          | Analógico | Botón respuesta “Sí” |
| NO (Cancelar)       | A1          | Analógico | Botón respuesta “No” |
| LCD SDA             | A4          | I2C | Comunicación LCD (SDA) |
| LCD SCL             | A5          | I2C | Comunicación LCD (SCL) |
| Switch principal    | VIN         | — | Alimentación y encendido general |
| GND común           | GND         | — | Tierra compartida |
| 5V común            | 5V          | — | Alimentación general |

> ⚙️ Todos los botones usan resistencias internas `INPUT_PULLUP` y se activan en **LOW**.
>  
> 🧩 La LCD se comunica mediante protocolo I2C (dirección **0x27**).

---

### 🔋 Alimentación
- Entrada recomendada: **5V** por USB o **9V** con regulador.
- Consumo estimado: **~100 mA** (LCD + botones).

---

### 🧠 Esquema base
- Conecta **GND** y **5V** del Arduino a las líneas del protoboard.  
- Los botones van entre **pin digital** y **GND**.  
- La LCD I2C se conecta directamente a A4 (SDA) y A5 (SCL).
