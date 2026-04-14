let ctx = document.getElementById('eegChart').getContext('2d');

// Define colors for each EEG channel
const colors = ['#00f0ff', '#ff6384', '#36a2eb', '#ffce56', '#4bc0c0', '#9966ff', '#ff9f40'];

// Create 7 datasets for 7 EEG channels
let eegChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: Array.from({ length: 30 }, (_, i) => i + 1),
        datasets: Array.from({ length: 7 }, (_, i) => ({
            label: `Channel ${i + 1}`,
            borderColor: colors[i],
            backgroundColor: 'transparent',
            data: Array(30).fill(0),
            tension: 0.4,
            pointRadius: 0,
            borderWidth: 2
        }))
    },
    options: {
        responsive: false,
        maintainAspectRatio: false,
        animation: false,
        scales: {
            y: {
                min: -1,
                max: 1
            }
        },
        plugins: {
            legend: {
                display: true
            }
        }
    }
});

let wheelchair = document.getElementById('wheelchair');
let arena = document.getElementById('arena');

function moveWheelchair(direction) {
    let step = 40;
    let x = wheelchair.offsetLeft;
    let y = wheelchair.offsetTop;

    if (direction === 'forward' && y - step >= 0) {
        wheelchair.style.top = (y - step) + 'px';
    } else if (direction === 'backward' && y + step <= (arena.clientHeight - wheelchair.clientHeight)) {
        wheelchair.style.top = (y + step) + 'px';
    } else if (direction === 'left' && x - step >= 0) {
        wheelchair.style.left = (x - step) + 'px';
    } else if (direction === 'right' && x + step <= (arena.clientWidth - wheelchair.clientWidth)) {
        wheelchair.style.left = (x + step) + 'px';
    }
}

function updateEEGSignal(wordsCount) {
    eegChart.data.datasets.forEach(dataset => {
        // Shift old data to left
        dataset.data.shift();

        // Add new data depending on number of words spoken
        let newValue = (Math.random() * wordsCount / 10) * (Math.random() < 0.5 ? 1 : -1);
        dataset.data.push(newValue.toFixed(2));
    });
    eegChart.update();
}

function sendCommandToCar(command) {
    fetch('/send_command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command: command })
    })
    .then(response => response.json())
    .then(data => {
        console.log("Sent to car:", data);
    })
    .catch(error => {
        console.error('Error sending command:', error);
    });
}

// Speech recognition
window.onload = function() {
    let recognition = new (window.SpeechRecognition || window.webkitSpeechRecognition)();
    recognition.continuous = true;
    recognition.lang = 'en-US';
    recognition.start();

    recognition.onresult = function(event) {
        let transcript = event.results[event.results.length - 1][0].transcript.trim().toLowerCase();
        console.log("Transcript:", transcript);

        // Calculate number of words
        let words = transcript.split(' ').filter(word => word.length > 0);
        let wordsCount = words.length || 1;

        // Update EEG based on number of words
        updateEEGSignal(wordsCount);

        // Check for negative words
        const negativeWords = ['not', "don't", 'do not', 'never', "can't"];
        let isNegative = negativeWords.some(neg => transcript.includes(neg));

        if (!isNegative) {
            if (transcript.includes('forward')) {
                moveWheelchair('forward');
                sendCommandToCar('forward');
                document.getElementById('wheelchairStatus').textContent = `🟢 Moving Forward`;
                document.getElementById('wheelchairStatus').style.color = 'lightgreen';
            }   
            if (transcript.includes('backward')) {
                moveWheelchair('backward');
                sendCommandToCar('backward');
                document.getElementById('wheelchairStatus').textContent = `🟢 Moving Backward`;
                document.getElementById('wheelchairStatus').style.color = 'lightgreen';
            }
            if (transcript.includes('left')) {
                moveWheelchair('left');
                sendCommandToCar('left');
                document.getElementById('wheelchairStatus').textContent = `🟢 Moving Left`;
                document.getElementById('wheelchairStatus').style.color = 'lightgreen';
            }
            if (transcript.includes('right')) {
                moveWheelchair('right');
                sendCommandToCar('right');
                document.getElementById('wheelchairStatus').textContent = `🟢 Moving Right`;
                document.getElementById('wheelchairStatus').style.color = 'lightgreen';
            }
            if (transcript.includes('stop')) {
                sendCommandToCar('stop');
                document.getElementById('wheelchairStatus').textContent = `🛑 Stopped`;
                document.getElementById('wheelchairStatus').style.color = 'yellow';
            }
        }

        document.getElementById('predictionText').textContent = transcript;
    };

    recognition.onerror = function(event) {
        console.error('Speech recognition error', event.error);
    };
}
