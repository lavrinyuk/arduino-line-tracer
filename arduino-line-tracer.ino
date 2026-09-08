// ==========================================
// НАСТРОЙКА ПИНОВ (PINOUT)
// ==========================================

// Датчики линии (цифровые выходы датчиков)
#define L_SENSOR 13
#define R_SENSOR 12

// Моторы — Управление скоростью (ШИМ / PWM)
#define L_MOTOR_PWM 3  // Подключается к ENA
#define R_MOTOR_PWM 5  // Подключается к ENB

// Моторы — Направление вращения
#define L_MOTOR_IN1 2  // IN1 на L298N
#define L_MOTOR_IN2 4  // IN2 на L298N
#define R_MOTOR_IN3 7  // IN3 на L298N
#define R_MOTOR_IN4 8  // IN4 на L298N

// ==========================================
// КОНСТАНТЫ И НАСТРОЙКИ
// ==========================================

// Скорость движения
#define BASE_SPEED 120

// Направления
#define FORWARD  false
#define BACKWARD true

#define LEFT_MOTOR  0
#define RIGHT_MOTOR 1

// Инверсия датчиков (Сейчас линия чёрная, датчики выдают LOW на белом)
const bool LINE = true; 

bool leftSensorState = false;
bool rightSensorState = false;

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(9600);

  // Настройка пинов датчиков
  pinMode(L_SENSOR, INPUT);
  pinMode(R_SENSOR, INPUT);

  // Настройка пинов драйвера L298N
  pinMode(L_MOTOR_PWM, OUTPUT);
  pinMode(R_MOTOR_PWM, OUTPUT);
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN3, OUTPUT);
  pinMode(R_MOTOR_IN4, OUTPUT);

  // Полная остановка при запуске
  stopMotors();
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  readSensors();

  // Логика движения по линии:
  // (HIGH = на линии/черном, LOW = на белом)

  if (!leftSensorState && !rightSensorState) {
    // Оба датчика на белом — едем прямо
    goForward(BASE_SPEED);
  } 
  else if (leftSensorState && !rightSensorState) {
    // Левый датчик увидал линию — поворачиваем влево
    turnLeft(BASE_SPEED);
  } 
  else if (!leftSensorState && rightSensorState) {
    // Правый датчик увидал линию — поворачиваем вправо
    turnRight(BASE_SPEED);
  } 
  else if (leftSensorState && rightSensorState) {
    // Оба датчика на линии (перекресток или стоп-линия) — останавливаемся
    stopMotors();
  }
}

// ==========================================
// ФУНКЦИИ УПРАВЛЕНИЯ МОТОРАМИ
// ==========================================

// Чтение состояния датчиков с учетом типа линии
void readSensors() {
  // Если LINE = true
  // Если LINE = false, инвертируем результат.
  leftSensorState  = digitalRead(L_SENSOR) == LINE;
  rightSensorState = digitalRead(R_SENSOR) == LINE;

  // Отладка
  // Serial.print("L: "); Serial.print(leftSensorState);
  // Serial.print(" | R: "); Serial.println(rightSensorState);
}

// Базовая функция управления мотором
void setMotor(int motor, bool dir, int speed) {
  // Защита от неправильных значений
  speed = constrain(speed, 0, 255);

  // dir/!dir - переключаем направление
  if (motor == LEFT_MOTOR) {
    digitalWrite(L_MOTOR_IN1, dir);
    digitalWrite(L_MOTOR_IN2, !dir);
    analogWrite(L_MOTOR_PWM, speed);
  } 
  else if (motor == RIGHT_MOTOR) {
    digitalWrite(R_MOTOR_IN3, dir);
    digitalWrite(R_MOTOR_IN4, !dir);
    analogWrite(R_MOTOR_PWM, speed);
  }
}

// Движение вперед
void goForward(int speed) {
  setMotor(LEFT_MOTOR, FORWARD, speed);
  setMotor(RIGHT_MOTOR, FORWARD, speed);
}

// Мягкий поворот влево (левое колесо стоит, правое едет)
void turnLeft(int speed) {
  setMotor(LEFT_MOTOR, FORWARD, 0);
  setMotor(RIGHT_MOTOR, FORWARD, speed);
}

// Мягкий поворот вправо (правое колесо стоит, левое едет)
void turnRight(int speed) {
  setMotor(LEFT_MOTOR, FORWARD, speed);
  setMotor(RIGHT_MOTOR, FORWARD, 0);
}

// Полная остановка (выключение ШИМ и отключение входов)
void stopMotors() {
  analogWrite(L_MOTOR_PWM, 0);
  analogWrite(R_MOTOR_PWM, 0);

  digitalWrite(L_MOTOR_IN1, LOW);
  digitalWrite(L_MOTOR_IN2, LOW);
  digitalWrite(R_MOTOR_IN3, LOW);
  digitalWrite(R_MOTOR_IN4, LOW);
}

// Тест
void testHardware() {
  Serial.println("--- ТЕСТ СТАРТ ---");

  Serial.println("1. Левый ВПЕРЕД");
  setMotor(LEFT_MOTOR, FORWARD, 90);
  delay(1000);
  stopMotors();

  Serial.println("2. Правый ВПЕРЕД");
  setMotor(RIGHT_MOTOR, FORWARD, 90);
  delay(1000);
  stopMotors();

  Serial.println("3. Оба НАЗАД");
  setMotor(LEFT_MOTOR, BACKWARD, 90);
  setMotor(RIGHT_MOTOR, BACKWARD, 90);
  delay(1000);

  stopMotors();
  Serial.println("4. Моторы остановлены");
  delay(500);

  // Считываем и показываем состояние датчиков
  readSensors();
  Serial.print("Датчики -> L: ");
  Serial.print(leftSensorState);
  Serial.print(" | R: ");
  Serial.println(rightSensorState);
  
  Serial.println("--- ТЕСТ ОКОНЧЕН ---");
  delay(2000);
}