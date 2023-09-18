import socket
from flask import Flask, request, jsonify

app = Flask(__name__)


def get_ip_address():
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    return ip_address


@app.route('/api/data', methods=['POST'])
def post_data():
    data = request.json
    print("Received data:", data)
    return jsonify({'message': 'Data received successfully!'}), 200


if __name__ == '__main__':
    ip_address = get_ip_address()
    print(f"Server starting at http://{ip_address}:5000/")
    app.run(host='0.0.0.0', port=5001)
