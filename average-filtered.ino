/*
 * Library Pembacaan Signal 4-20mA untuk Arduino
 * Dibuat untuk stabilkan pembacaan analog input
 * 
 * Fitur:
 * - Moving Average Filter
 * - Auto-scaling ke mA
 * - Mudah dipakai
 */

// ============================================
// KONFIGURASI (EDIT DI SINI)
// ============================================
const int ANALOG_PIN = A1;           // Pin analog yang dipakai
const int MIN_RAW = 186;             // Nilai ADC minimum (4mA)
const int MAX_RAW = 945;             // Nilai ADC maximum (20mA) - SESUAIKAN!
const float MIN_MA = 4.0;            // Arus minimum (mA)
const float MAX_MA = 20.0;           // Arus maximum (mA)
const int NUM_SAMPLES = 10;          // Jumlah sampel untuk averaging (makin besar makin smooth tapi lambat)

// ============================================
// VARIABEL INTERNAL (JANGAN EDIT)
// ============================================
int readings[NUM_SAMPLES];
int readIndex = 0;
long total = 0;
int average = 0;
bool initialized = false;

// ============================================
// FUNGSI UTAMA
// ============================================

/**
 * Inisialisasi filter - WAJIB dipanggil di setup()
 */
void initFilter() {
  for (int i = 0; i < NUM_SAMPLES; i++) {
    readings[i] = 0;
  }
  readIndex = 0;
  total = 0;
  initialized = true;
}

/**
 * Baca nilai analog yang sudah di-filter
 * Return: nilai ADC rata-rata (raw)
 */
int readFilteredRaw() {
  if (!initialized) {
    initFilter(); // Auto-init jika lupa
  }
  
  // Kurangi nilai lama
  total = total - readings[readIndex];
  
  // Baca nilai baru
  readings[readIndex] = analogRead(ANALOG_PIN);
  
  // Tambahkan ke total
  total = total + readings[readIndex];
  
  // Pindah index
  readIndex++;
  if (readIndex >= NUM_SAMPLES) {
    readIndex = 0;
  }
  
  // Hitung rata-rata
  average = total / NUM_SAMPLES;
  
  return average;
}

/**
 * Baca nilai dalam mA (sudah di-convert)
 * Return: arus dalam mA (float)
 */
float readCurrent_mA() {
  int raw = readFilteredRaw();
  
  // Constrain agar tidak keluar range
  raw = constrain(raw, MIN_RAW, MAX_RAW);
  
  // Map ke 4-20mA
  float current = map(raw, MIN_RAW, MAX_RAW, MIN_MA * 100, MAX_MA * 100) / 100.0;
  
  return current;
}

/**
 * Baca nilai dalam persen (0-100%)
 * Return: persentase 0-100%
 */
float readPercent() {
  float current = readCurrent_mA();
  float percent = ((current - MIN_MA) / (MAX_MA - MIN_MA)) * 100.0;
  percent = constrain(percent, 0, 100);
  return percent;
}

/**
 * Get raw value terakhir (tanpa filter)
 * Return: nilai ADC langsung
 */
int readRawDirect() {
  return analogRead(ANALOG_PIN);
}

/**
 * Reset filter (mulai dari awal)
 */
void resetFilter() {
  initFilter();
}

// ============================================
// CONTOH PENGGUNAAN
// ============================================

void setup() {
  Serial.begin(9600);
  
  // WAJIB: Inisialisasi filter
  initFilter();
  
  Serial.println("=================================");
  Serial.println("Signal 4-20mA Reader - READY!");
  Serial.println("=================================");
}

void loop() {
  /*
  // ========================================
  // CARA 1: Baca langsung dalam mA (PALING MUDAH)
  // ========================================
  float current = readCurrent_mA();
  
  Serial.print("Current: ");
  Serial.print(current, 2);  // 2 digit desimal
  Serial.println(" mA");
  */
  
  // ========================================
  // CARA 2: Baca dalam persen
  // ========================================
  /*
  float percent = readPercent();
  
  Serial.print("Level: ");
  Serial.print(percent, 1);
  Serial.println(" %");
  */
  
  
  // ========================================
  // CARA 3: Baca raw value (untuk debugging)
  // ========================================
  
  int raw = readFilteredRaw();
  int direct = readRawDirect();
  
  Serial.print("Raw (filtered): ");
  Serial.print(raw);
  Serial.print(" | Raw (direct): ");
  Serial.println(direct);
  
  
  
  // ========================================
  // CARA 4: Baca semua sekaligus (untuk monitoring)
  // ========================================
  /*
  int raw = readFilteredRaw();
  float current = readCurrent_mA();
  float percent = readPercent();
  
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print(" | Current: ");
  Serial.print(current, 2);
  Serial.print(" mA | Level: ");
  Serial.print(percent, 1);
  Serial.println(" %");
  */
  
  delay(100);  // Update setiap 100ms
}
