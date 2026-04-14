# M.E.N.T.A.L — Brain‑Computer Interface (BCI) Wheelchair Control System

## _Overview_

**M.E.N.T.A.L (Mind Enhanced Neurotechnological Adaptive Learning)** is a Brain‑Computer Interface **(BCI)** system designed to enable hands‑free wheelchair control for **disabled** people using **EEG** (electroencephalography) signals. The system classifies mental commands and translates them into movement directions such as **forward**, **backwards,** **left**, **right**, and **stop**.

The project uses a clinical‑grade BRAIN QUICK® EEG System with Natus® NeuroWorks® software for data acquisition, along with our own **hybrid CNN‑LSTM deep learning model** for classification. It was supervised by **Dr. Ahmar Rashid** and **Dr. Shahab Ansari**.

---

## _Project_ _Objectives_

- Acquire EEG signals using the NeuroWorks system for mental‑command recognition.
- Develop a hybrid CNN‑LSTM architecture for accurate EEG classification.
- Build a real‑time system for wheelchair control using Arduino controller.
- Implement a robust preprocessing and signal‑cleaning pipeline.

---

## _System Architecture_

### 1. **Data Acquisition (EEG)**

EEG data was collected using the **NeuroWorks clinical EEG device**, which provides high‑resolution multi‑channel recordings. The signals captured include:

- Delta (0.5–4 Hz)
- Theta (4–8 Hz)
- Alpha (8–13 Hz)
- Beta (13–30 Hz)
- Gamma (30+ Hz)

These frequency bands are associated with mental tasks such as concentration, motor imagery, cognitive processing, and relaxation.

---

### 2. **Preprocessing Pipeline**

Because EEG signals contain noise and artifacts, the following preprocessing steps were used:

- **Band‑pass filtering** (1–45 Hz)
- **Notch filter** for 50/60 Hz removal
- **Independent Component Analysis (ICA)** for blink and muscle artifact removal
- **Segmentation** into fixed‑length windows
- **Normalization** to reduce subject‑to‑subject variability

---

### 3. **Feature Engineering / Representation**

Although the model learns features automatically, additional representations may include:

- Power Spectral Density (PSD)
- Short‑Time Fourier Transform (STFT)
- Statistical descriptors per channel

For the final pipeline, the hybrid DL model primarily uses **raw or minimally processed temporal EEG segments** as input.

---

### 4. **AI Model Architecture — CNN‑LSTM Hybrid**

The final selected architecture is a **CNN‑LSTM Hybrid Model**, chosen for its ability to capture both spatial patterns and temporal dynamics in EEG data.

#### 🔹 **CNN Component**

- Learns spatial correlations between EEG channels.
- Extracts frequency‑dependent patterns.
- Helps reduce noise through learned filters.

#### 🔹 **LSTM Component**

- Models temporal evolution of signals across time.
- Effective for sequential data such as EEG.
- Improves command‑recognition stability.

#### 🔹 **Why CNN‑LSTM?**

- CNN handles spatial + frequency features.
- LSTM handles time‑dependent mental‑state changes.
- Combined, they outperform standalone CNN or RNN models for EEG classification.

---

##  *Evaluation Metrics*

- **Accuracy** of mental‑command prediction
- **Macro F1 Score** for balanced evaluation
- **Latency** (< 700 ms required for reliable and immediate **wheelchair** control for **disabled** people)

---

## _Technologies Used_

- Python (NumPy, SciPy, Pandas)
- TensorFlow (Deep Learning)
- MNE Toolkit (EEG preprocessing)
- Scikit‑Learn (Classical ML utilities)
- Matplotlib (Visualizations)
- Arduino (Wheelchair Control)

## License

This project is for academic and research purposes under university guidelines.
