import { green, emphasisDisabled } from '../../../../../../utilities/colors';

type Props = {
  isActive: boolean;
};

const Indicator = (props: Props) => {
  const { isActive } = props;

  return (
    <div
      style={{
        backgroundColor: isActive ? green : emphasisDisabled,
        width: '12px',
        margin: 'auto 16px auto 8px',
        height: '12px',
        borderRadius: '50%',
      }}
    />
  );
};

export default Indicator;
