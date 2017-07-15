import React from 'react'

let nextTextId = 0
class AddForm extends React.Component {
    constructor(props) {
        super(props)

        this.state = {
            text: ''
        }

        this.handleTitleChange = this.handleTitleChange.bind(this)
        this.handleTextAdded = this.handleTextAdded.bind(this)
    }

    handleTitleChange(e) {
        this.setState({text: e.target.value})
    }

    handleTextAdded(e) {
        e.preventDefault()
        if (this.state.text.trim() !== '') {
            this.props.onAddButtonClick({id: nextTextId++, text: this.state.text.trim()})
        }
        this.setState({text: ''})
    }

    render() {
        console.log('AddForm.render')
        return (
          <form onSubmit={this.handleTextAdded}>
            <input type="text" value={this.state.text} onChange={this.handleTitleChange}/>
            <button type="submit">Add</button>
          </form>
        )
    }
}

export default AddForm
