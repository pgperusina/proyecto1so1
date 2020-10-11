import React, { Component } from 'react';
import './App.css';

class App extends Component {
  state = {
    mensajes: []
  }
  componentDidMount() {
    this.interval = setInterval(() => this.getData(), 5000);
  }
  getData() {
    fetch('http://35.192.42.127/getDocuments')
      .then(res => res.json())
      .then((data) => {
        this.setState({ mensajes: data.results })
        console.log(this.state.mensajes)
      })
      .catch(console.log)
  }
  render() {
    return (
      <div className="container">
        <div className="col-xs-12">
          <h1>Publicaciones</h1>
          {this.state.mensajes.map((mensaje) => (
            <div className="card">
              <div className="card-body">
                <h5 className="card-title">{mensaje.usuario}</h5>
                <h6 className="card-subtitle mb-2 text-muted">
                  {mensaje.mensaje}
                </h6>
              </div>
            </div>
          ))}
        </div>
      </div>
    );
  }
}

export default App;
