import React from 'react'

import AddForm from "./AddForm"
import TextList from "./TextList"

export default class Add extends React.Component {
    constructor (props) {
        super(props);

        this.state = {
            texts: []
        }

        this.handleAddButtonClick = this.handleAddButtonClick.bind(this);
    }

    handleAddButtonClick(text) {
        this.setState({texts: [text, ...this.state.texts]});
    }

    render() {
        console.log('Add.render: ', this.state.texts);
        return (
          <div>
            <AddForm onAddButtonClick={this.handleAddButtonClick} />
            <TextList texts={this.state.texts}/>
          </div>
        );
    }
}
