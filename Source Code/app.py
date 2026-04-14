from flask import Flask, render_template, request, jsonify
import serial
import time
import numpy as np
import pandas as pd
from tensorflow.keras.models import load_model
import os


app = Flask(__name__)

# --- Bluetooth Setup ---
# try:
#     bluetooth_serial = serial.Serial('COM11', 9600)
#     time.sleep(2)
    
#     # Test the connection immediately
#     bluetooth_serial.write(b'\n')  # Send dummy byte
#     print("Bluetooth connected and ready!")
    
# except Exception as e:
#     bluetooth_serial = None
#     print(f"Bluetooth connection failed: {e}")

# --- Command mappings ---
command_mapping = {
    'stop': '0',
    'forward': '1',
    'backward': '2',
    'left': '3',
    'right': '4',
}

# --- Load model and EEG encoder ---
try:
    model = load_model('model.h5')

    # Load EEG datasets
    user_a = pd.read_csv('thinkoutloud1.csv')
    user_d = pd.read_csv('thinkoutloud2.csv')

    # Merge datasets
    eeg_data = pd.concat([user_a, user_d], ignore_index=True)

    # Prepare classwise samples
    static_eeg_samples = {
        0: eeg_data[eeg_data.iloc[:, -1] == 0].iloc[0, :-1].values,  # stop
        1: eeg_data[eeg_data.iloc[:, -1] == 1].iloc[0, :-1].values,  # forward
        2: eeg_data[eeg_data.iloc[:, -1] == 2].iloc[0, :-1].values,  # backward
        3: eeg_data[eeg_data.iloc[:, -1] == 3].iloc[0, :-1].values,  # left
        4: eeg_data[eeg_data.iloc[:, -1] == 4].iloc[0, :-1].values,  # right
    }

    print("Static EEG encoder ready!")

except Exception as e:
    model = None
    static_eeg_samples = {}
    print(f"Error loading model or EEG samples: {e}")

# --- Encoder: Voice to EEG ---
def encode_command_to_eeg(command):
    """
    Given a voice command, pick EEG signal
    and pass through model.
    """
    mapping = {
        'stop': 0,
        'forward': 1,
        'backward': 2,
        'left': 3,
        'right': 4,
    }
    class_id = mapping.get(command.lower())

    if class_id is not None and static_eeg_samples:
        eeg_sample = static_eeg_samples[class_id].reshape(1, -1)
        _ = model.predict(eeg_sample, verbose=0)  # Pass through model
        print(f"Encoded and passed EEG sample for: {command}")

# --- Flask routes ---

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/send_command', methods=['POST'])
def send_command():
    if bluetooth_serial is None:
        return jsonify({'status': 'error', 'message': 'Bluetooth not connected'})

    data = request.json
    command = data.get('command', '')
    mapped_command = command_mapping.get(command.lower())

    if mapped_command:
        encode_command_to_eeg(command)

        # Send actual command to car
        bluetooth_serial.write((mapped_command + '\n').encode())
        return jsonify({'status': 'success', 'sent': mapped_command})
    else:
        return jsonify({'status': 'error', 'message': 'Invalid command received'})

# --- Main ---

if __name__ == '__main__':
    if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
        # Only connect Bluetooth in real Flask server run
        try:
            bluetooth_serial = serial.Serial('COM11', 9600)
            time.sleep(2)
            bluetooth_serial.write(b'\n')
            print("Bluetooth connected and ready!")
        except Exception as e:
            bluetooth_serial = None
            print(f"Bluetooth connection failed: {e}")
    else:
        bluetooth_serial = None  # Dummy for first run

    app.run(debug=True)


