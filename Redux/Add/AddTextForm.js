import React from 'react'
import {connect} from 'react-redux'

let nextTextId = 0
const addText = (text) => ({
  type: 'ADD_TEXT',
  text: {
    id: nextTextId++,
    text
  }
})

let AddTextForm = ({dispatch}) => {
    let input
    return (
      <div>
        <form onSubmit={e => {
          e.preventDefault()
          if (!input.value.trim()) {
            return
          }
          dispatch(addText(input.value.trim()))
          input.value = ''
        }}>
          <input ref={node => {
            input = node
          }} />
          <button type="submit">
              Add
          </button>
        </form>
      </div>
    )
}


AddTextForm = connect()(AddTextForm)

export default AddTextForm
