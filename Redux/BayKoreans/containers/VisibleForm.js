import React from 'react'
import {connect} from 'react-redux'

import {addBookmark, fetchShow} from '../actions'
import {Show} from "../Show"

export const midsMap = {
    'drama': '방영 드라마',
    'drama_fin': '종영 드라마',
    'current': '시사|교양',
    'entertain': '연예|오락',
};
const defaultMid = 'current'
let mid = defaultMid

const Form = ({dispatch}) => {
    let select;
    let input;

    return (
      <form onSubmit={e => {
        e.preventDefault()
        dispatch(fetchShow(new Show(select.value, input.value.trim())))
      }}>
        <select ref={node => {select = node}} value={mid} onChange={e => mid = e.target.value}>
            {Object.keys(midsMap).map((mid) =>
              <option key={mid} value={mid}>{midsMap[mid]}</option>
            )}
        </select>
        <input ref={node => {
          input = node
        }} />
        <button type="submit">Search</button>
        <button onClick={e => {
            e.preventDefault()
            dispatch(addBookmark(new Show(select.value, input.value.trim())))
        }}>bookmark</button>
      </form>
    )
};


const VisibleForm = connect()(Form);

export default VisibleForm;
